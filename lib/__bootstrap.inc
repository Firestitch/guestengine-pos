<?
	error_reporting(E_ALL ^ E_STRICT);

	include_once("APPLICATION.inc");
	include_once("util/MISC_UTIL.inc");
	include_once("util/DEBUG_UTIL.inc");
	include_once("util/FILE_UTIL.inc");
	include_once("util/SERVER_UTIL.inc");
	include_once("util/ZIP_UTIL.inc");
	include_once("util/UNZIP_UTIL.inc");
	include_once("util/SSH_UTIL.inc");
	include_once("util/SFTP_UTIL.inc");
	include_once("util/EMAIL_UTIL.inc");
	include_once("util/LOG_UTIL.inc");
	include_once("models/processor_complex_model.inc");	
	include_once("models/maitred_processor_complex_model.inc");
	include_once("models/pixelpoint_processor_complex_model.inc");

	APPLICATION::init();

	


