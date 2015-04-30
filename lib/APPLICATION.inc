<?
	class APPLICATION {

		static $instance 		= null;
		
		protected $_log_util 	= null;
		protected $_verbose 		= false;

		public function __construct() {

			date_default_timezone_set("America/Toronto");

			FILE_UTIL::mkdir($this->get_data_dir());
			FILE_UTIL::mkdir($this->get_log_dir());

			$this->_log_util = LOG_UTIL::create($this->get_log_dir()."sync-".date("Y-m-d").".log");			
		}

		public function init() {

			if(!self::$instance)
				self::$instance = new APPLICATION();

			return self::$instance;
		}

		public function instance() {
			return self::$instance;
		}

		protected $_options = array();

		public function options($short,$long=array()) {
			$this->_options = getopt($short,$long);
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
			$email_util->set_smtp_pass("q1w2e3r4t5y6");
			$email_util->set_smtp_port(587);	

			return $email_util;
		}	
	}

	
