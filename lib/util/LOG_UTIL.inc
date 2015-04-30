<?
	
	class LOG_UTIL {
        
		protected $_timestamp_format 	= "%Y-%m-%d %H:%M:%S";
      	protected $_file 				= null;
		protected $_debug 				= false;              
		protected $_prefix	 			= "[%Y-%m-%d %H:%M:%S {microseconds}] {level}";

		public function __construct($file) {
			$this->_file = $file;
		}
             
		public function create($file) {
			return new LOG_UTIL($file);
		}

		public function debug() {
			$this->_debug = true;
		}

		public function log($data,$level="") {

			if($this->_debug)
				echo $data."\n";
		
			/*
			$filesize = FILE_UTIL::get_filesize($this->get_log_file());
			
			if($filesize>($this->_rotate_limit_bites*1000)) {
				
				preg_match("/(.*?)(?:\.([^\.]+)|)$/",$this->get_log_file(),$matches);
				
				$basename = get_value($matches,1);
				$ext 	  = get_value($matches,2);
				
				$rotate_file = $basename."-".strftime("%Y-%m-%d").($ext ? ".".$ext : "");
				
				FILE_UTIL::rename($this->get_log_file(),$rotate_file);
			}
			*/
			
	  		$mtime = explode(" ", microtime());
	    		$msec = number_format(get_value($mtime,0),8);
	    		
			$replaces["{microseconds}"] 	= $msec;
			$replaces["{level}"] 			= $level ? "[".$level."] " : "";
				
			$prefix = str_replace(array_keys($replaces),array_values($replaces),strftime($this->_prefix));
			
			$log = $prefix.$data;
			$fileh = @fopen($this->_file, "a");
			
			if($fileh)
				@fwrite($fileh, $log."\r\n");
		}
	}
