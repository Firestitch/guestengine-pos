<?
	abstract class CMODEL_PROCESSOR extends CMODEL {

		protected $_application 	= null;
		protected $_host 			= null;
		protected $_user 			= null;
		protected $_key 			= null;
		protected $_password		= null;
		protected $_path 			= null;
		protected $_data_dir 		= null;
		protected $_purge 			= null;
		protected $_archive			= null;
		protected $_zip_util		= null;
		protected $_processed_files	= array();
		
		public function __construct($host,$user,$key,$password,$data_dir,$path,$purge,$archive) {

			$this->_application = APPLICATION::instance();
			$this->_host 		= $host;
			$this->_user 		= $user;
			$this->_password	= $password;
			$this->_path 		= $path;
			$this->_purge 		= $purge;
			$this->_archive		= $archive;
			$this->_data_dir 	= $data_dir;

			FILE_UTIL::mkdir($this->application()->get_log_dir());
			FILE_UTIL::mkdir($this->application()->get_archive_dir());

			$this->_db = new CMODEL_DB($this->_application->get_data_dir(),$purge);
		}

		public function log($message) 	{ $this->_application->log($message); }
		public function data_dir() 		{ return $this->_data_dir; }
		public function db() 			{ return $this->_db; }
		public function path() 			{ return $this->_path; }
		public function password() 		{ return $this->_password; }
		public function user() 			{ return $this->_user; }
		public function purge() 		{ return $this->_purge; }
		public function archive() 		{ return $this->_archive; }
		public function key() 			{ return $this->_key; }
		public function host() 			{ return $this->_host; }
		public function application() 	{ return $this->_application; }	

		public function add_file($file,$path,$store) {	

			$fp = @fopen($file,"r+");

			if($fp && @flock($fp,LOCK_EX)) {

				if(!$this->db()->exists($store,basename($file))) {
					$this->_zip_util->add_file($file,$path);
					$add_file = str_replace($this->data_dir(),"",$file);
					$this->log("Added File: ".$add_file." @ ".$store);
				}
				
				$this->_processed_files[] = array(	"file"=>$file,
													"filename"=>basename($file),
													"store"=>$store);
			}			
			
			@fclose($fp);
		}

		public function file($store,$filename) { return $tbis->data_dir()."store/".$filename; }

		public static function create($pos,$host,$user,$key,$password,$data_dir,$path,$purge,$archive) {

			if(!is_dir($data_dir))
				throw new Exception("The data directory does not exist");

			if(!in_array($pos,array("maitred","pixelpoint","guestengine")))
				throw new Exception("Invalid POS type (Supports maitred and pixelpoint)");		

			if($pos=="maitred")
				return new CMODEL_MAITRED_PROCESSOR($host,$user,$key,$password,$data_dir,$path,$purge,$archive);

			if($pos=="pixelpoint")
				return new CMODEL_PIXELPOINT_PROCESSOR($host,$user,$key,$password,$data_dir,$path,$purge,$archive);
			
			if($pos=="guestengine")
				return new CMODEL_GUESTENGINE_PROCESSOR($host,$user,$key,$password,$data_dir,$path,$purge,$archive);			

			throw new Exception("Invalid POS type");			
		}

		public function process() {

			FILE_UTIL::mkdir($this->application()->get_tmp_dir());

			$zip_file = $this->application()->get_tmp_dir()."pos-".date("Y-m-d")."-".time().".zip";
			$this->_zip_util = new ZIP_UTIL($zip_file);

			$this->log("Looking for new files...");

			$this->build();

			$this->log("Saving zip file...");

			$this->_zip_util->close();			

			if(is_file($zip_file)) {

				if($this->host()) {

					$this->log("Uploading to ".$this->host()."...");

					$final_file 	= basename($zip_file);
					$upload_file 	= $final_file.".upload";

					$sftp_util = new SFTP_UTIL($this->host());	
					
					if($this->key())
						$sftp_util->connect_key($this->user(),$this->key());
					else
						$sftp_util->connect_password($this->user(),$this->password());

					$sftp_util
						->cd($this->path())
						->put($upload_file,$zip_file)
						->rename($upload_file,$final_file);
				}
				
				if($this->archive()) {
					$this->log("Archiving zip file ".basename($zip_file));
					FILE_UTIL::move($zip_file,$this->application()->get_archive_dir().basename($zip_file));
				}

			} else
				$this->log("There are no new files to send");

			FILE_UTIL::delete($zip_file);
			
			$this->log("Successfully completed sync");
			
			if($this->purge()) {

				$this->log("Purging xml files...");

				foreach($this->_processed_files as $processed_file)
					FILE_UTIL::delete(get_value($processed_file,"file"));
			} 
			
			$this->db()->clear();

			foreach($this->_processed_files as $processed_file)
				$this->db()->log(get_value($processed_file,"store"),get_value($processed_file,"filename"));
			
			$this->db()->save();
		}

		abstract public function build();
	}

	class CMODEL_DB {

		protected $_data_file 	= null;
		protected $_db_dir 		= null;
		protected $_purge 		= null;
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
