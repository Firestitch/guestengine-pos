<?
	/**
	* Class SFTP_UTIL
	*
	* @package Framework
	* @subpackage Utilities
	*/
	
	class SFTP_UTIL extends SSH_UTIL {
        
        /**
         * Constructor
         * @param string $host
         * @param integer $port
         * @throws Exception 
         */
		public function __construct($host, $port=22) {

			if(!$host)
				throw new Exception("No host address supplied");				
			
			APPLICATION::instance()->add_include_path(APPLICATION::instance()->get_packages_dir()."phpseclib/");

			require_once("Net/SFTP.php");		
			require_once("Crypt/RSA.php");

			$this->start_error_handler();

			$this->_connection = new Net_SFTP($host);

			$this->complete_error_handler();
		}
                
        /**
         * Exec
         * @param string $exec 
         */
		public function exec($exec) {}

        /**
         * pwd
         * @return string 
         */
		public function pwd() { return $this->_connection->pwd(); }

        /**
         * Put
         * @param string $remote_file
         * @param array $data
         * @return boolean
         * @throws Exception 
         */
		public function put($remote_file,$data) {

			$option = is_file($data) ? NET_SFTP_LOCAL_FILE : null;

			$data = str_replace(array("\r\n","\r"),array("\n",""),$data);

			$remote_dir 		= dirname($remote_file);
			$remote_filename 	= basename($remote_file);

			$this->start_error_handler();

			if($remote_dir)
				if(!$this->_connection->chdir($remote_dir))
					throw new Exception($this->_connection->getLastSFTPError().": ".$remote_dir);

			$return = $this->_connection->put($remote_filename,$data,$option);
			$this->complete_error_handler();

			if(!$return)
				throw new Exception("Failed to put file: ".$this->_connection->getLastSFTPError());
			
			//echo $this->_connection->getSFTPLog();

			return $this;
		}

		public function cd($path) {

			$this->start_error_handler();

			$return = $this->_connection->chdir($path);
			
			$this->complete_error_handler();

			return $this;
		}
		
		public function listing($path) {

			$this->start_error_handler();

			$return = $this->_connection->nlist($path);
			
			$this->complete_error_handler();

			return $return;
		}

		public function rename($from,$to) {

			$this->start_error_handler();

			$return = $this->_connection->rename($from,$to);
			
			$this->complete_error_handler();

			if(!$return)
				throw new Exception("Failed to rename file: ".$this->_connection->getLastSFTPError());
			
			return $this;		
		}

		public function delete($file) {

			$this->start_error_handler();

			$return = $this->_connection->delete($file);
			
			$this->complete_error_handler();

			if(!$return)
				throw new Exception("Failed to delete file: ".$this->_connection->getLastSFTPError());
			
			return $this;
		}		
	}
