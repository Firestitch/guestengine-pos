<? 
	require("./lib/__bootstrap.inc");
	echo "\n";

	$application = APPLICATION::instance();
	$application->options("",array("file:","host:","user:","key:","password:","path:"));


	$file 		= $application->option("file");
	$path 		= FILE_UTIL::get_sanitized_directory($application->option("path"),"/");
	$password	= $application->option("password");
	$host 		= $application->option("host");
	$user 		= $application->option("user");
	$key_file	= $application->option("key");
	$key 		= FILE_UTIL::get($key_file);

	echo "\n";
	echo "File:           ".(is_file($file) ? "Exists" : "Does not exist")."\n";
	echo "SSH Host:       ".$host."\n";
	echo "SSH Username:   ".$user."\n";
	echo "SSH Key:        ".(is_file($key_file) ? "Exists" : "Does not exist")."\n";
	echo "SSH Path:       ".$path."\n";
	
	if(is_file($file)) {

		try {
			echo "\nConnecting...\n\n";

			$sftp_util = new SFTP_UTIL($host);	

			if($password)
				$sftp_util->connect_password($user,$password);
			else
				$sftp_util->connect_key($user,$key);

			$sftp_util->cd($path);

			$final_file 	= basename($file);
			$upload_file 	= $final_file.".upload";
			
			echo "Uploading zip file ".$final_file."...\n\n";

			$sftp_util->put($upload_file,$file);
			$sftp_util->rename($upload_file,$final_file);

			echo "Upload Successful\n\n";

		} catch(Exception $e) {
			echo "SSH Connection: Failed - ".$e->getMessage()."\n";
		}
	}

	echo "\n";
