<?php
    /**
    * Class SERVER_UTIL
    *
    * @package Framework
    * @subpackage Utilities
    */

	class SERVER_UTIL {
	
		const LOCALHOST_IP = 2130706433;

		/**
		 * Checks the operating system is windows
		 *
		 * @return string
		 */
		 
		static function is_os_windows() {
			return substr(PHP_OS,0,3) == 'WIN';
		}
		
		/**
		 * Checks the operating system is linux
		 *
		 * @return string
		 */
		 
		static function is_os_linux() {
			return stripos(PHP_OS,"linux")!==false;
		}	
		
        /**
         * Returns hostname
         * @return string 
         */
		static function get_hostname() {
			return php_uname("n");
		}			
		
		/**
		 * Gets redirect url
		 *
		 * @return string
		 */
		 
		static function get_redirect_url() 		{ return get_value($_SERVER,"REDIRECT_URL");	}

		/**
		 * Gets request uri
		 *
		 * @return string
		 */
		 
		static function get_request_uri() 		{ return get_value($_SERVER,"REQUEST_URI");	}	

		/**
		 * Gets redirect query string
		 *
		 * @return string
		 */
		 
		static function get_redirect_query_string() 		{ return get_value($_SERVER,"REDIRECT_QUERY_STRING");	}	

		/**
		 * Gets document root
		 *
		 * @return string
		 */
		 
		static function get_document_root() 		{ return get_value($_SERVER,"DOCUMENT_ROOT");	}		
		
        /**
         * Returns the request url
         * @return string 
         */
		static function get_request_url($remove_parms=array("SESSID")) {
				
			$uri = self::get_request_uri();

			foreach($remove_parms as $remove_parm)
				$uri = preg_replace("/".$remove_parm."[:=][^&:]+/","",$uri);

			$uri = preg_replace("/\/{2,}/","/",$uri);

			return "http://".self::get_server_host().$uri;
		}	

		/**
		 * Gets remote ip address
		 *
		 * @return string
		 */
		 
		static function get_remote_ip() {
		
			if(isset($_SERVER)) { 
				
				$ip = get_value($_SERVER,"HTTP_X_CLUSTER_CLIENT_IP"); 
				
				if(!$ip) 
					$ip = get_value($_SERVER,"HTTP_X_REAL_IP"); 
				
				if(!$ip) 
					$ip = get_value($_SERVER,"HTTP_X_FORWARDED_FOR"); 
					
				if(!$ip) 
					$ip = get_value($_SERVER,"HTTP_CLIENT_IP"); 
					
				if(!$ip) 
					$ip = get_value($_SERVER,"REMOTE_ADDR"); 
			} else { 
				if(getenv("HTTP_X_CLUSTER_CLIENT_IP"))
					$ip = getenv("HTTP_X_CLUSTER_CLIENT_IP"); 
				
				elseif(getenv("HTTP_X_REAL_IP"))
					$ip = getenv("HTTP_X_REAL_IP"); 
					
				elseif(getenv("HTTP_X_FORWARDED_FOR"))
					$ip = getenv("HTTP_X_FORWARDED_FOR"); 
				
				elseif(getenv("HTTP_CLIENT_IP")) 
					$ip = getenv("HTTP_CLIENT_IP"); 
				else
					$ip = getenv("REMOTE_ADDR"); 
			} 
			
			return $ip; 
		}

        /**
         * Returns user agent
         * @return mixed 
         */
		static function get_user_agent() 		{ return get_value($_SERVER,"HTTP_USER_AGENT");	}

		/**
		 * Returns the cookie
		 * @return mixed 
		 */
		static function get_cookie() 			{ return get_value($_SERVER,"HTTP_COOKIE");	}	

		/**
		 * Returns referer
		 * @return mixed 
		 */
		static function get_referer() 			{ return get_value($_SERVER,"HTTP_REFERER");	}

		/**
		 * Returns the accepted language
		 * @return mixed
		 */
		static function get_accept_language() 		{ return get_value($_SERVER,"HTTP_ACCEPT_LANGUAGE");	}	
			
		
		/**
		 * Gets remote ip address
		 *
		 * @return string
		 */
		 
		static function get_remote_binary_ip() 		{ return CONVERT_UTIL::get_binary_ip_address(self::get_remote_ip()); }
		
		/**
		 * Gets the script name
		 *
		 * @return string
		 */
		 
		static function get_script_name() 		{ return get_value($_SERVER,"SCRIPT_NAME"); }

		/**
		 * Gets server name
		 *
		 * @return string
		 */
		 
		static function get_server_name() 		{ return get_value($_SERVER,"SERVER_NAME"); }

		/**
		 * Gets server host
		 *
		 * @return string
		 */
		 
		static function get_server_host() 		{ return get_value($_SERVER,"HTTP_HOST"); }
		
		/**
		 * Gets server domain
		 *
		 * @return string
		 */
		 
		static function get_system_domain() {
			preg_match("/[^\.]+\.[a-z]+$/",self::get_server_host(),$matches); 
			return get_value($matches,0);
		}
		
		/**
		 * Checks server https
		 *
		 * @return boolean
		 */
		 
		static function is_server_https() {
			return strcasecmp(get_value($_SERVER,"HTTPS"),"on")==0 || strcasecmp(get_value($_SERVER,"HTTP_X_HTTPS"),"on")==0 || strtolower(get_value($_SERVER,"HTTP_X_FORWARDED_PROTO"))=="https"; 
		}
                
        /**
         * Check whether server is http
         * @return boolean 
         */
		static function is_server_http() 		{ return !self::is_server_https(); }

		/**
		 * Gets server protocol
		 *
		 * @return string
		 */
		 
		static function get_server_protocol() 		{ return self::is_server_https() ? "https" : "http"; }

        /**
         * Sets the memory limit
         * @param integer $mb 
         */
		static function set_memory_limit($mb) 		{ ini_set("memory_limit", $mb."M"); }
		
        /**
         * Sets the display errors
         * @param integer $value 
         */
		static function set_display_errors($value) 	{ ini_set("display_errors", (int)$value); }
		
        /**
         * Sets the error reporting
         * @param string $value 
         */
		static function set_error_reporting($value) 	{ error_reporting($value); }
		
        /**
         * Returns the error reporting
         * @return integer 
         */
		static function get_error_reporting() 		{ return error_reporting(); }		
		
		/**
		 * Gets the apache version
		 *
		 * @return string
		 */
		 
		static function get_apache_version() {
			$apache_version = apache_get_version();
			$pattern = '/Apache\/([0-9]+.[0-9]+.[0-9]+)/';	
			preg_match ($pattern, $apache_version,$matches);
			return get_value($matches,1);
		}

		/**
		 * Gets the php version
		 *
		 * @return string
		 */
		 
		static function get_php_version() {
			$pattern = '/([0-9]+.[0-9]+.[0-9]+)/';	
			preg_match ($pattern, PHP_VERSION,$matches);
			return get_value($matches,1);
		}	
		
		/**
		 * Checks web application
		 *
		 * @return boolean
		 */
		 
		static function is_web_application() {
			return isset($_SERVER["HTTP_ACCEPT"]) || isset($_SERVER["HTTP_KEEP_ALIVE"]) || isset($_SERVER["HTTP_ACCEPT_CHARSET"]);
		}
		
		/**
		 * Checks is command application
		 *
		 * @return boolean
		 */
		 
		static function is_command_application() {
			return !SERVER_UTIL::is_web_application();
		}

		/**
		 * Sets the max execution 
		 *
		 * @param int $minutes int minutes
		 * @return boolean
		 */
		 
		static function set_max_execution_minutes($minutes) {
			$time = 60 * 60 * $minutes;
			
			set_time_limit($time);
			ini_set("max_execution_time",$time);
		}

		/**
		 * Sets the max input 
		 *
		 * @param int $minutes int minutes
		 * @param boolean $max_input_time boolean enables max input time
		 * @return boolean
		 */
		 
		static function set_max_input_minutes($minutes) {
			$time = 60 * 60 * $minutes;
			ini_set("max_input_time",$time);
		}

		/**
		 * Todo: Ray
		 */
		 
		static function add_include_path_pear() {
			self::add_include_path(DIR_FRAMEWORK."packages/Pear/pear/");	
		}

        /**
         * Adds include path horde 
         */
		static function add_include_path_horde() {
			self::add_include_path(DIR_FRAMEWORK."packages/Pear/pear/Horde/");	
		}		
        
        /**
         * The exec command
         * @param string $cmd
         * @param array $output
         * @param boolean $capture_output
         * @param string $return_var
         * @return boolean 
         */
		static function exec($cmd,&$output=array(),$capture_output=false,&$return_var=null) {
			
			if($capture_output)
				$cmd = $cmd." 2>&1";
			
			exec($cmd,$output,$return_var);
			
			return !$return_var;
		}

        /**
         * Shell exec
         * @param string $cmd
         * @param boolean $capture_output
         * @return string 
         */
		static function shell_exec($cmd,$capture_output=false) {
			if($capture_output)
				$cmd = $cmd." 2>&1";
				
			return shell_exec($cmd);
		}

        /**
         * Adds include path
         * @param string $path 
         */
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
		
		/**
		 * Gets ghostscript executable
		 *
		 * @return string
		 */
		 
		static function get_ghostscript_executable() { return self::is_os_windows() ? "gswin32c" : "gs"; }
		
		/**
		 * Returns the max file size
		 *
		 * @return string
		 */
		
		static function get_max_upload_mb() {
			return (int)self::get_max_filesize();
		}
		
        /**
         * Returns the max filesize
         * @return integer 
         */
		static function get_max_filesize() {

			$max_filesize 	= ini_get("upload_max_filesize");
			$post_max_size 	= ini_get("post_max_size");

			return (int)$post_max_size<(int)$max_filesize ? $post_max_size : $max_filesize;
			
		}

		static function is_post() {
			return strtoupper(get_value($_SERVER,"REQUEST_METHOD"))=="POST";
		}
	}
	
	
	
	
	
	