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

		$time = strtotime("2015-03-28");

		foreach($sqlsrv->select("SELECT StoreNo,StoreName FROM EC_StoreExport") as $store) {

			$store_number = value($store,"StoreNo");

			$store_dir = $data_dir.$store_number."/";

			FILE_UTIL::mkdir($store_dir);

			$time_cmodel = CMODEL_TIME::create($time)->round_day();

			for($day=0;$day<7;$day++) {

				$xml_file = $store_dir."invoice-".$time_cmodel->db_date().".xml";
				
				$xml_writer_util = 
				XML_WRITER_UTIL::create()
				->open("Location",array("code"=>$store_number,"name"=>value($store,"StoreName"),"date"=>$time_cmodel->db_date()))
					->open("Invoices");

				$sql = "SELECT CheckID,CheckNo,OpenDate,FirstName,LastName,EmpID FROM Y_CheckHeader
		  				INNER JOIN K_Employee ON K_Employee.EmpID = Y_CheckHeader.SettledID  
		  				WHERE StoreNo = ".$store_number." AND TransactionDate = '".$time_cmodel->db()."'";
					
				$invoices = $sqlsrv->select($sql);

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

						$item_plu = $plu ? value($item,"PLU") : value($item,"MenuID");
						$xml_writer_util
							->open("Item",array("plu"=>$item_plu))
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

				$xml_writer_util->save($xml_file);
				$time_cmodel->subtract_day(1);
			}		
		}

	} catch(Exception $e) {
		$application->handle_exception($e,"Extract POS Error: ");
	}	