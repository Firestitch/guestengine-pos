<? 
	require(dirname(__FILE__)."/lib/__bootstrap.inc");
	
	$application = APPLICATION::instance();
	$application->options("",array("host:","user:","pos:","data:","key:","password:","path:","archive","verbose","purge","validate"));

	try {
		
		$data_dir 	= FILE_UTIL::get_sanitized_directory($application->option("data"));
		$path 		= FILE_UTIL::get_sanitized_directory($application->option("path"),"/");
		$host 		= $application->option("host");
		$user 		= $application->option("user");
		$password	= $application->option("password");
		$pos 		= $application->option("pos");
		$verbose 	= !is_null($application->option("verbose"));
		$purge 		= !is_null($application->option("purge"));
		$validate 	= !is_null($application->option("validate"));
		$archive 	= !is_null($application->option("archive"));
		$key_file	= $application->option("key");
		$key 		= FILE_UTIL::get($key_file);

		if($verbose)
			$application->verbose();

		if($validate || $verbose) {
			echo "\n";
			echo "Validate:       ".($validate ? "Yes" : "No")."\n";
			echo "Data Directory: ".(is_dir($data_dir) ? $data_dir : "Does not exist")."\n";
			echo "POS Type:       ".(in_array($pos,array("maitred","pixelpoint","guestengine")) ? $pos : "Invalid")."\n";
			echo "SSH Host:       ".($host ? $host : "Not supplied")."\n";
			echo "SSH Username:   ".($user ? $user : "Not supplied")."\n";
			echo "SSH Key:        ".($key_file ? (is_file($key_file) ? "Exists" : "Supplied, but not found") : "Not Supplied")."\n";
			echo "SSH Password:   ".($password ? "Exists" : "Not supplied")."\n";
			echo "SSH Path:       ".($path ? $path : "Not supplied")."\n";
			echo "Purge:          ".($purge ? "Yes" : "No")."\n";
			echo "Archive:        ".($archive ? "Yes" : "No")."\n\n";
		}

		if($validate) {
			try {
				
				$sftp_util = new SFTP_UTIL($host);

				if($key)
					$sftp_util->connect_key($user,$key);
				else
					$sftp_util->connect_password($user,$password);

				echo "Connection Successful\n\n";

				$sftp_util->cd($path);
				echo "Change Directory Successful\n\n";

				echo "Listing...\n\n";
				foreach($sftp_util->listing(".") as $file)
					echo $file."\n";
				echo "\n";			

				$sftp_util->put("test","TEST");
				echo "Put Successful\n\n";

				//$sftp_util->delete("test");
				//echo "Delete Successful\n\n";
			
			} catch(Exception $e) {
				echo "SSH Failed - ".$e->getMessage()."\n";
			}

			die("\n");
		}
		
		$processor_cmodel = CMODEL_PROCESSOR::create($pos,$host,$user,$key,$password,$data_dir,$path,$purge,$archive);
		$processor_cmodel->process();

	} catch(Exception $e) {
		$application->handle_exception($e,"POS Sync Error: ");
	}