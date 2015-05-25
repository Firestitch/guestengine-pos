<?
	error_reporting(E_ALL ^ E_STRICT);

	include_once("APPLICATION.inc");
	include_once("util/MISC_UTIL.inc");
	include_once("constant.inc");
	include_once("registry.inc");
	include_once("uri.inc");
	include_once("models/complex_model.inc");

	function autoload($class_name) {

		if(preg_match("/^CMODEL_(.*)/",$class_name,$matches))
			require_once("models/".strtolower(value($matches,1))."_complex_model.inc");

		if(preg_match("/_UTIL$/",$class_name))
			require_once("util/".$class_name.".inc");		
	}

	spl_autoload_register("autoload");
	APPLICATION::init();

	echo "\n";	
