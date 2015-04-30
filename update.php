<? 
	require("./lib/__bootstrap.inc");
	echo "\n";

	$application = APPLICATION::instance();
	$application->options("s:h:u:k:",array());

	$url = "http://1b33be46f2a1d08f226c-b75cb1fc63a7b556ce67820e1878dd8f.r63.cf2.rackcdn.com/application.zip";
	
	$fu = fopen($url,"rb");

	if(!$fu)
		die("Error opening URL");
	
	FILE_UTIL::mkdir($application->get_tmp_dir());
	$zip_file = $application->get_tmp_dir()."application.zip";

	$fp = fopen($zip_file,"w");

	while(!feof($fu)) {		
		$content = fread($fu,2048);
		fwrite($fp, $content);
	}
	
	fclose($fp);	
	fclose($fu);

	$unzip_util = UNZIP_UTIL::create($zip_file);

	$unzip_util->extract($application->get_instance_dir());

	FILE_UTIL::delete($zip_file);