<? 
	require(dirname(__FILE__)."/lib/__bootstrap.inc");
	
	//https://www.microsoft.com/en-us/download/confirmation.aspx?id=29065
	//Microsoft® SQL Server® 2012 Native Client - sqlncli.msi

	$application = APPLICATION::instance();
	$application->options("",array("system:","plu:","data:"));

	$data_dir 	= $application->option("data") ? $application->option("data") : REGISTRY::instance()->get("data");
	$plu 		= $application->option("plu") ? $application->option("plu") : REGISTRY::instance()->get("plu");
	$system 	= $application->option("system") ? $application->option("system") : REGISTRY::instance()->get("system");
	
	try {

		$data_dir = FILE_UTIL::get_sanitized_directory($data_dir);

		if(!is_dir($data_dir))
			throw new Exception("Invalid data directory");

		if(!in_array($system,array("squirrel")))
			throw new Exception("Invalid data directory");		
		
		$sqlsrv = CMODEL_SQLSRV::create()->connect();
		$time = time();

		$stores = $sqlsrv->select("SELECT StoreNo,StoreName FROM EC_StoreExport");
		
		echo "Starting extraction\n";
		echo "Found ".count($stores)." stores\n";

		foreach($stores as $store) {

			$store_number = value($store,"StoreNo");

			$store_dir = $data_dir.$store_number."/";

			FILE_UTIL::mkdir($store_dir);

			$time_cmodel = CMODEL_TIME::create($time)->round_day();

			for($day=0;$day<7;$day++) {

				if($day)
					$time_cmodel->subtract_day(1);

				$filename ="invoice-".$time_cmodel->db_date().".xml";
				$xml_file = $store_dir.$filename;

				if(is_file($xml_file)) {
					echo "Invoice ".$filename." already exists";
					continue;
				}
				
				$xml_writer_util = 
				XML_WRITER_UTIL::create()
				->open("Location",array("code"=>$store_number,"name"=>value($store,"StoreName"),"date"=>$time_cmodel->db_date()))
					->open("Invoices");

				$sql = "SELECT CheckID,CheckNo,OpenDate,J_Employee.FirstName,J_Employee.LastName,J_Employee.EmpID FROM Y_CheckHeader
		  				INNER JOIN J_Employee ON J_Employee.EmpID = Y_CheckHeader.SettledID AND J_Employee.StoreNo = Y_CheckHeader.StoreNo
		  				WHERE Y_CheckHeader.StoreNo = ".$store_number." AND TransactionDate = '".$time_cmodel-&gt;db()."'";
					
				$invoices = $sqlsrv->select($sql);

				if(!$invoices) {
					echo "No Invoices: Store ".$store_number." on ".$time_cmodel->db_date()."\n";
					continue;
				}

				foreach($invoices as $invoice) {

					$xml_writer_util
						->open("Invoice",array("code"=>value($invoice,"CheckNo")))
							->open("Stats")
								->add("Total","")
								->add("Subtotal","")
								->add("Meals","")
								->add("OpenDate",value($invoice,"OpenDate")->format('Y-m-d\TH:i:s'))
							->close("Stats")
							->open("Staff",array("code"=>value($invoice,"EmpID")))
								->add("Name",trim(value($invoice,"FirstName")." ".value($invoice,"LastName")))
							->close("Staff")
							->open("Items");

					$sql = "SELECT K_Menu.MenuID, K_Menu.CatID, PLU, K_Menu.Name as ItemName, K_Category.Name as CatName, OriginalPrice, Quantity FROM Y_CheckItem
			  				INNER JOIN K_Menu ON K_Menu.MenuID = Y_CheckItem.MenuID
		  					INNER JOIN K_Category ON K_Category.CatID = K_Menu.CatID
		  					WHERE CheckID = ".value($invoice,"CheckID")." AND StoreNo = ".$store_number;
			
					$items = $sqlsrv->select($sql);

					foreach($items as $item) {

						$xml_writer_util
							->open("Item",array("plu"=>value($item,"PLU"),"id"=>value($item,"MenuID")))
								->add("Description",value($item,"ItemName"))
								->add("Quantity",value($item,"Quantity"))
								->add("Price",value($item,"OriginalPrice"))
								->add("Meal",value($item,0))
								->open("Category",array("code"=>value($item,"CatID")))
									->add("Name",value($item,"CatName"))
								->close("Category")
							->close("Item");
					}

					$xml_writer_util
						->close("Items")
						->close("Invoice");
				}

				$xml_writer_util						
					->close("Invoices")
				->close("Location");

				echo "Created Store #".$store_number." ".$filename."\n";

				$xml_writer_util->save($xml_file);				
			}		
		}

		echo "Extraction Completed\n";
		
	} catch(Exception $e) {
		$application->handle_exception($e,"Extract POS Error: ");
	}	