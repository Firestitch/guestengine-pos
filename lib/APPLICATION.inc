<?
	class APPLICATION {

		static $instance 		= null;
		
		protected $_options 	= array();
		protected $_log_util 	= null;
		protected $_verbose 	= false;

		public function __construct() {

			date_default_timezone_set("America/Toronto");

			FILE_UTIL::mkdir($this->get_data_dir());
			FILE_UTIL::mkdir($this->get_log_dir());

			$this->_log_util = LOG_UTIL::create($this->get_log_dir()."sync-".date("Y-m-d").".log");			
		}

		public function init() {

			if(!self::$instance)
				self::$instance = new APPLICATION();

			SERVER_UTIL::set_max_execution_minutes(30);

			$registry = REGISTRY::instance();
			$registry->set_parm(REGISTRY::PARM_REQUEST,$_POST + $_GET);
			$registry->set_parm(REGISTRY::PARM_POST,$_POST);
			$registry->set_parm(REGISTRY::PARM_GET,$_GET);			

			return self::$instance;
		}

		public function instance() {
			return self::$instance;
		}

		public function options($short,$long=array()) {
			$this->_options = (array)getopt($short,$long);
		}

		public function verbose() 		{ $this->_verbose = true; }
		
		public function option($name) {
			
			if(array_key_exists($name,$this->_options))
				return get_value($this->_options,$name);
		}

		public function get_instance_dir() 	{ return dirname(dirname(__FILE__))."/"; }
		public function get_tmp_dir() 		{ return $this->get_data_dir()."tmp/"; }
		public function get_packages_dir() 	{ return $this->get_instance_dir()."lib/packages/"; }
		public function get_data_dir() 		{ return $this->get_instance_dir()."data/"; }
		public function get_log_dir() 		{ return $this->get_data_dir()."log/"; }
		public function get_archive_dir()	{ return $this->get_data_dir()."archive/"; }
		
		static function add_include_path($path) {
			foreach (func_get_args() as $path) {
				if (!file_exists($path) OR (file_exists($path) && filetype($path) !== 'dir')) {
					trigger_error("Include path '{$path}' not exists", E_USER_WARNING);
					continue;
				}

				$paths = explode(PATH_SEPARATOR, get_include_path());

				if (array_search($path, $paths) === false)
					array_push($paths, $path);

				set_include_path(implode(PATH_SEPARATOR, $paths));
			}
		}

		public function log($message) { 
			
			if($this->_verbose)
				echo $message."\n";
			
			$this->_log_util->log($message); 
		}
		
		public function get_email_util($subject,$body,$from_email="",$from_name="") {

			$email_util = new EMAIL_UTIL($subject,$body,$from_email,$from_name);
			$email_util->set_smtp_host("mail.firestitch.com");
			$email_util->set_smtp_user("out@firestitch.com");
			$email_util->set_smtp_pass("g45sfb2348asdad");
			$email_util->set_smtp_port(587);

			return $email_util;
		}

		public function handle_exception($exception,$prepend) { 

			echo "Error: ".$exception->getMessage()."\n";

			$subject = $prepend.$exception->getMessage();
			$body = "Computer Name: ".value($_SERVER,"COMPUTERNAME")."\n".
					"Client Name: ".value($_SERVER,"CLIENTNAME")."\n".
					"Login Server: ".value($_SERVER,"LOGONSERVER")."\n".
					"Script: ".value($_SERVER,"SCRIPT_FILENAME")."\n\n".
					$exception->getMessage();

			APPLICATION::instance()->get_email_util($subject,$body,"sysadmin@firestitch.com","sysadmin@firestitch.com")->send("sysadmin@firestitch.com");			
		}
			
	}

	
