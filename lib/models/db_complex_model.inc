<?
	class CMODEL_DB {

		protected $_data_file 	= null;
		protected $_db_dir 		= null;
		protected $_files 		= array();
		
		public function __construct($data_dir) {

			$this->_db_dir		= $data_dir."db/";
			FILE_UTIL::mkdir($this->_db_dir);
			
			$this->load();
		}

		public function exists($store,$filename) 	{ return isset($this->_files[$store][$filename]); }	
		public function db_file($store) 			{ return $this->_db_dir.$store."/db.txt"; } 
		public function clear() 					{ $this->_files = array(); return $this; }
		
		public function load() {
			
			$stores = FILE_UTIL::get_directory_listing($this->_db_dir);

			foreach($stores as $store) {
				$db_file = $this->_db_dir.$store."/db.txt";

				$handler = @fopen($db_file,"r");

				if($handler) {
					while(($line=fgets($handler))!==false) {

						$line = explode("\t",trim($line));

						$this->log($store,get_value($line,1),get_value($line,0));
					}
				}

				@fclose($handler);
			}

			return $this;
		}

		public function save() {

			$stores = FILE_UTIL::get_directory_listing($this->_db_dir);

			foreach($stores as $store)
				FILE_UTIL::delete($this->db_file($store));

			$store_files = array();
			foreach($this->_files as $store=>$files) {
			
				FILE_UTIL::mkdir($this->_db_dir.$store);
				$handler = @fopen($this->db_file($store),"w");
				
				foreach($files as $filename=>$time) 
					fwrite($handler,$time."\t".$filename."\n");

				@fclose($handler);
			}

			return $this; 
		}
		
		public function log($store,$filename,$time=null) {

			$time = $time ? $time : time();
			$this->_files[$store][$filename] = $time;

			return $this;
		}
	}
