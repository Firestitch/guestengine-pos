<?php
    /**
    * Class DEBUG_UTIL
    *
    * @package Framework
    * @subpackage Utilities
    */

	class DEBUG_UTIL {
	
		const OUTPUT_FORMAT_HTML 			= "H";
		const OUTPUT_FORMAT_TEXT 			= "T";
		
		/**
		 * SHUTDOWN CALLBACK
		 * @var string $SHUTDOWN_CALLBACK 
		 */
		static $_SHUTDOWN_CALLBACK			= null;

		/**
		 * LAST ERROR
		 * @var array $LAST_ERROR 
		 */                
		static $_LAST_ERROR		 			= array();

		/**
		 * PREVIOUS ERROR HANDLER
		 * @var string $PREVIOUS_ERROR_HANDLER 
		 */
		static $_PREVIOUS_ERROR_HANDLER 	= "";

		/**
		 * OUTPUT FORMAT
		 * @var string $OUTPUT_FORMAT 
		 */
		static $_OUTPUT_FORMAT				= self::OUTPUT_FORMAT_HTML;

		/**
		 * REPORTABLE ERRORS
		 * @var array $REPORTABLE_ERRORS 
		 */
		static $_REPORTABLE_ERRORS			= array(E_ERROR=>true, 
													E_WARNING=>true,
													E_PARSE=>true,
													E_NOTICE=>true,
													E_CORE_ERROR=>true,
													E_CORE_WARNING=>true,
													E_COMPILE_ERROR=>true,
													E_COMPILE_WARNING=>true,
													E_USER_ERROR=>true,
													E_USER_WARNING=>true,
													E_USER_NOTICE=>true);
			
		/**
		 * Checks debug output format is text
	 	 */
		
		static function is_format_text() 		{ return self::$_OUTPUT_FORMAT==self::OUTPUT_FORMAT_TEXT; }
		
		/**
		 * Checks debug output format is html
	 	 */
	 	
		static function is_format_html() 		{ return self::$_OUTPUT_FORMAT==self::OUTPUT_FORMAT_HTML; }
		
		/**
		 * Enables debug output format to text
	 	 */
		
		static function enable_format_text() 		{ self::format_text(); }
        
        /**
         * Format text 
         */
		static function format_text() 			{ self::$_OUTPUT_FORMAT = self::OUTPUT_FORMAT_TEXT; }
		
		/**
		 * Enables debug output format to html
	 	 */
	 	 
		static function enable_format_html() 		{ self::format_html(); }
                
        /**
         * Format html 
         */
		static function format_html() 			{ self::$_OUTPUT_FORMAT = self::OUTPUT_FORMAT_HTML; }
		
        /**
         * Returns the last error
         * @return array 
         */
		static function get_last_error() 		{ return self::$_LAST_ERROR; }
		
		/**
		 * Sets the reportable errors to bits
		 *
		 * @param array $bits 
	 	 */
		
		static function set_reportable_errors($bits)	{ self::$_REPORTABLE_ERRORS = $bits; }
		
		/**
		 * Prints out the stack trace
		 *
		 * @param integer $stack_trace_steps
		 * @param string $format
	 	 */
		
		static function print_stack_trace($stack_trace_steps=0,$format=null) {
		
			$format = $format ? $format : self::$_OUTPUT_FORMAT;
			
			$stack_trace = self::get_stack_trace(++$stack_trace_steps);
			
			$table = self::get_backtrace_table("", "", "", "", $stack_trace, $format);
			$table->render();
		}
		
		/**
		 * Prints out a list of procedures from start to finish , showing step by step, leading up to the point of break 
		 *
		 * @param integer $start_trace_index
		 * @return string
	 	 */
		
		static function get_stack_trace($start_trace_index=0,$trace=array()) {	
			
			$trace = $trace ? $trace : debug_backtrace();
			
			array_shift($trace);
			
			$traces = array();
			
			for($index=$start_trace_index;$index<count($trace);$index++) {
				
				$backtrace = get_value($trace,$index);
				
				$arguments = get_value($backtrace,"args",array());				
				
				$function_arguments = array();

				foreach($arguments as $argument) {

					$type = gettype($argument);
					
					if($type=="integer" || $type=="double")
						$function_arguments[] = $argument;
						
					elseif($type=="string") {
						
						$argument = htmlspecialchars(STRING_UTIL::get_dot_dot_dot_string($argument,64));
						$function_arguments[] = '"'.str_replace("\\","/",$argument).'"';
					
					} elseif($type=="array")
						$function_arguments[] = "Array(".count($argument).")";
					
					elseif($type=="object")
						$function_arguments[] = 'Object('.get_class($argument).")";
								
					elseif($type=="resource")
						$function_arguments[] = "Resource(".$argument.")";
					
					elseif($type=="boolean")
						$function_arguments[] = $argument ? "True" : "False";
					
					elseif($type=="NULL")
						$function_arguments[] = "null";
					
					else
						$function_arguments[] = "Unknown";									
				}
				
				$filename 	= get_value($backtrace,"file","N/A");
				$filename 	= str_replace("\\","/",$filename);
				$line_number 	= get_value($backtrace,"line");
				$class 		= get_value($backtrace,"class");
				$function 	= get_value($backtrace,"function");
				$type 		= get_value($backtrace,"type");
				
				$traces[] = array("line_number"=>$line_number,"file"=>$filename,"class"=>$class,"type"=>$type,"function"=>$function,"function_arguments"=>$function_arguments);				
			}
			
		   	return $traces;
		}
		
		/**
		 * Enables an email to be sent the recipient when an error is recovered
		 *
	 	 */
		
		static function enable_email_error_notify() {		
			self::$_PREVIOUS_ERROR_HANDLER = set_error_handler(array("DEBUG_UTIL","email_error_handler"));
		}
	
		/**
		 * Checks whether the error is reportable and sends an email to the recipient
		 *
		 * @param integer $errno integer value of the error type 
		 * @param string $errmsg details on what the error is
		 * @param string $filename file that the error occured
		 * @param int $linenum line number on which the error occured
		 * @return string
		 */

		static function email_error_handler($errno, $errmsg, $filename, $linenum) {

			if(!DEBUG_UTIL::is_reportable($errno,$errmsg)) 
				return false;

			DEBUG_UTIL::email_error($errno, $errmsg, $filename, $linenum);
			DEBUG_UTIL::log_error($errno, $errmsg, $filename, $linenum);
		}		
		
		/**
         * Enables system error handler
         * @param string $class
         * @param string $function 
         */
		static function enable_system_error_handler($class="DEBUG_UTIL",$function="system_error_handler") {
			self::$_PREVIOUS_ERROR_HANDLER = set_error_handler(array($class,$function));
		}
		
		/**
		 * Checks whether the error is reportable and displays it
		 *
		 * @param int $errno integer value of the error type 
		 * @param string $errmsg details on what the error is
		 * @param string $filename file that the error occured
		 * @param int $linenum line number on which the error occured
		 * @return string
		 */

		static function system_error_handler($errno, $errmsg, $filename, $linenum) {		
			
			if(!DEBUG_UTIL::is_reportable($errno,$errmsg)) 
				return false;

			self::$_LAST_ERROR = array("type"=>$errno,"message"=>$errmsg,"file"=>$filename,"line"=>$linenum);

			DEBUG_UTIL::display_error($errno, $errmsg, $filename, $linenum);

			DEBUG_UTIL::log_error($errno, $errmsg, $filename, $linenum);
		}

		
		/**
		 * Enables exception error handling whihc throws exceptions for legacy functions that echo out error messages
	 	 */
		
		static function enable_exception_error_handler() {
			self::$_PREVIOUS_ERROR_HANDLER = set_error_handler(array("DEBUG_UTIL","exception_error_handler"));
		}

        /**
         *  Disables exception error handler
         */
		static function disable_exception_error_handler() {
			
			if(self::$_PREVIOUS_ERROR_HANDLER)
				set_error_handler(self::$_PREVIOUS_ERROR_HANDLER);
		}		
		
		/**
		 * Throws an exception for all error messages
		 *
		 * @param int $errno integer value of the error type 
		 * @param string $errmsg details on what the error is
		 * @param string $filename file that the error occured
		 * @param int $linenum line number on which the error occured
		 * @return null
		 */

		static function exception_error_handler($errno, $errmsg, $filename, $linenum) {

			DEBUG_UTIL::disable_exception_error_handler();

			throw new Exception($errmsg,$errno);
		}		
		
		
                /**
                 * Returns the line
                 */
		static function get_line() {
			return str_repeat("-",100);
		}
		

				
		/**
		 * Displays information about the error 
		 *
		 * @param integer $errno integer value of the error type 
		 * @param string $errmsg details on what the error is
                 * @param string $filename
		 * @param integer $linenum line number on which the error occured
                 * @param string $stack_trace
		 * @param string $format output format (default = html)
		 * @return array
                 * @todo review types
	 	 */
		
		static function get_backtrace_table($errno, $errmsg, $filename, $linenum, $stack_trace=null,$format=self::OUTPUT_FORMAT_HTML) {
			
			if($errmsg) {
				$data[] = array($errmsg);
				$data[] = array($filename." @ ".$linenum);
			}
			
			if($format==self::OUTPUT_FORMAT_TEXT) {
				
				$data[] = array("");
				$data[] = array("");

				foreach($stack_trace as $index=>$trace) {

					$line_number 			= get_value($trace,"line_number");
					$file 					= get_value($trace,"file");
					$class 					= get_value($trace,"class");
					$type 					= get_value($trace,"type");
					$function 				= get_value($trace,"function");
					$function_arguments 	= get_value($trace,"function_arguments");

					$data[] = array($file." @ ".$line_number);
					$data[] = array($class.$type.$function."(".implode(",",$function_arguments).")");
					$data[] = array("");
				}
				
				$data[] = array("");	

			} elseif($format==self::OUTPUT_FORMAT_HTML) {

				$backtrace = "";

				$data[] = array("<br>");

				foreach($stack_trace as $trace) {

					$line_number 		= get_value($trace,"line_number");
					$file 			= get_value($trace,"file");
					$class 			= get_value($trace,"class");
					$type 			= get_value($trace,"type");
					$function 		= get_value($trace,"function");
					$function_arguments 	= get_value($trace,"function_arguments");

					$backtrace .= 	$class.$type.$function."(".implode(",",$function_arguments).")<br>".								
									HTML_UTIL::get_link("file://".$file,$file)." @ ".$line_number."<br><br>";				
				}

				$data[] = array($backtrace);
			}
			
			if($format==self::OUTPUT_FORMAT_HTML) {
	
				$table = new HTML_TABLE_UTIL();
				$table->set_data($data);			
				$table->disable_css();
			} else {

				$table = new TEXT_TABLE_UTIL();
				$table->set_data($data);
				$table->disable_borders();
				$table->set_column_attributes(0,array("width"=>12));
				$table->set_column_attributes(1,array("width"=>88));				
			}

			return $table;					
		}
		
		/**
		 * Returns a list of error codes
		 *
		 * @return array
	 	 */

		static function get_error_type_list() {
			return array(	E_ERROR=> "Error",
							E_WARNING=> "Warning",
							E_PARSE=> "Parsing Error",
							E_NOTICE=> "Notice",
							E_CORE_ERROR=> "Core Error",
							E_CORE_WARNING=> "Core Warning",
							E_COMPILE_ERROR	=> "Compile Error",
							E_COMPILE_WARNING=> "Compile Warning",
							E_USER_ERROR=> "User Error",
							E_USER_WARNING=> "User Warning",
							E_USER_NOTICE=> "User Notice",
							E_STRICT=> "Runtime Notice");
		}
	
		
		/**
		 * Besure for all registerd functions that they are all set to a public scope
                 * @param boolean $handler
		 */
		static function enable_fatal_error_handler($handler) {

			if($handler) {
				
				DEBUG_UTIL::$_SHUTDOWN_CALLBACK = new SHUTDOWN_CALLBACK($handler);
				
				register_shutdown_function(array(DEBUG_UTIL::$_SHUTDOWN_CALLBACK,"call"));
			}
		}
                
                /**
                 * Disable fatal error handler
                 */
		static function disable_fatal_error_handler() {
			
			if(DEBUG_UTIL::$_SHUTDOWN_CALLBACK)
				DEBUG_UTIL::$_SHUTDOWN_CALLBACK->unregister();
		}
		
                /**
                 *  Email last error
                 */
		public static function email_last_error() {
			
			if($error=error_get_last()) {
			 				 	
			 	$file 		= get_value($error,"file");
			 	$line 		= get_value($error,"line");
			 	$type 		= get_value($error,"type");
			 	$message 	= get_value($error,"message");
			 
			 	if(self::is_reportable($type,$message)) 
			 		self::email_error($type, $message, $file, $line);
			 }
		}

		/**
		 * Determines whether the error should be displayed
		 *
		 * @param int $errno error code being checked
		 * @param string $errmsg error message
         * @param array $reportable_errors
		 * @return string
	 	 */

		static function is_reportable($errno,$errmsg,$reportable_errors=array()) {

			if(strpos($errmsg,"headers already sent")!==false) return false;
			
			$reportable_errors = $reportable_errors ? $reportable_errors : array_keys(self::$_REPORTABLE_ERRORS);

			if(!in_array($errno,$reportable_errors)) 
				return false;
			
			//If the error reporting is set to zero or a function has a @ placed infront of it
			if(error_reporting()==0)
				return false;

			return true;
		}
		

        /**
         * last_error reportable check
         * @return boolean 
         */
		static public function is_last_error_reportable() {	

			//If the error reporting is set to zero or a function has a @ placed infront of it
			if(error_reporting()==0)
				return false;
			
			$errno = get_value(error_get_last(),"type");

			return self::is_reportable($errno,"");
		}

		/**
		 * Displays the backtrace table 
		 *
	 	 * @param int $errno integer value of the error type 
		 * @param string $errmsg details on what the error is
                 * @param string $filename
		 * @param int $linenum line number on which the error occured
		 * @param string $stack_trace_steps 
		 * @param string $format output format (default = html)
		 * @return string
	 	 */
		
		static function get_debugtrace($errno,$errmsg,$filename,$linenum,$stack_trace_steps,$format=self::OUTPUT_FORMAT_HTML,$stack_trace=array()) {

			//DB trigger error - correct
			//CMODEL error - correct
			$stack_trace = $stack_trace ? $stack_trace : self::get_stack_trace($stack_trace_steps);

			$pointer = 0;
			foreach($stack_trace as $index=>$trace) {
				if(	get_value($trace,"function")=="system_error_handler" || 
					(get_value($trace,"class")=="MODEL_COMPLEX" && get_value($trace,"function")=="__call"))
						$pointer = $index + 1;
			}
		
			$stack_trace = array_slice($stack_trace, $pointer);
					
			if(strpos($errmsg,"Call to undefined CMODEL method")!==false) {
				$trace = array_shift($stack_trace);

				$linenum = get_value($trace,"line_number");
				$filename = get_value($trace,"file");
			}	

			$filename = FILE_UTIL::get_sanitized_path($filename);

			return self::get_backtrace_table($errno, $errmsg, $filename, $linenum, $stack_trace, $format)->get_table();							
		}
	

		public static function print_r($array,$return=true,$steps=0) { 
			$s = "";

			$level = get_value(ob_get_status(),"level");

			if($level==0)
				return print_r($array,true);

			$type = "";

			if(is_array($array) || is_object($array)) {

				if(is_array($array))
					$type = "Array";

				elseif(is_object($array)) {
					
					$reflect = new ReflectionClass($array);

					$props = $reflect->getProperties();

					$object = $array;
					$array = array();
					foreach($props as $key=>$value) {

						$name = $value->name;
						$array[$name] = $value->class;
					}

					$type = "Object";
				}
	
				$s = $type."\n".str_repeat(" ",($steps) * 8)."(\n";

				foreach($array as $name=>$value) {

					$val = "";

					if(is_array($value))
						$val = self::print_r($value,$return,($steps + 1));

					elseif(is_object($value)) {
						$name = $value->name;

						$val = self::print_r($array->$name,$return,($steps + 1));

					} elseif(!is_resource($value))
					 	$val = $value."\n";

					$s .= str_repeat(" ",($steps * 7) + 4)."[".$name."] => ".$val;
				}

				$s .= str_repeat(" ",($steps) * 8).")\n";
			}

			if(!$return)
				echo $s;

			return $s;
		} 


		static function email_error_exception($exception) {		

			$trace = $exception->getTrace();

			array_shift($trace);

			$trace = self::get_stack_trace(0,$trace);

			self::email_error("",$exception->getMessage(),$exception->getFile(),$exception->getLine(),$trace);
		}

		
		/**
		 * Sends the error message to the recipient 
		 *
		 * @param int $errno integer value of the error type 
		 * @param string $errmsg details on what the error is
         * @param string $filename
		 * @param int $linenum line number on which the error occured
	 	 */
		
		static function email_error($errno,$errmsg,$filename,$linenum,$trace=array()) {
			
			$message = array();
			$message[] = "Date Time: ".date("F j, Y, g:i:s a");
			
			if(!SERVER_UTIL::is_command_application())
				$message[]  = "URL: ".SERVER_UTIL::get_request_url();
				
			$message[] = "Server Hostname: ".SERVER_UTIL::get_hostname();
			$message[] = "Memory Usage: ".FORMAT_UTIL::get_formatted_filesize(memory_get_usage());
			
			$message[] = "";
			
			$message[] = self::get_debugtrace($errno,$errmsg,$filename,$linenum,2,self::OUTPUT_FORMAT_TEXT,$trace);
			
			$message[] = '$_SERVER';
			$message[] = self::get_line();
			
			$message[] = self::print_r($_SERVER);
			$message[] = "";

			if($_POST) {
				$message[] = "\$_POST\n".self::get_line();									
				$message[] = self::print_r($_POST);
			}			

			if($_GET) {
				$message[] = "\$_GET\n".self::get_line();								
				$message[] = self::print_r($_GET);
			}
			
			if($_FILES) {
				$message[] = '\$_FILES';
				$message[] = self::get_line();								
				$message[] = self::print_r($_FILES);
			}
			
			if(SERVER_UTIL::is_os_linux()) {
				
				SERVER_UTIL::exec("top -b -n1",$output);
				
				$message[] = "Top";
				$message[] = self::get_line();				
				$message[] = self::print_r($output);
			}		
						
			$notify_recipients = SYSTEM_MANAGER::get_notify_recipients();
		
			$email_cmodel = SYSTEM_MANAGER::get_email();
			
			$has_success = false;
			
			if($email_cmodel) {
				
				if($notify_recipients) {
				
					$temp_dir = BASE_MODEL_PATH::get_temporary_directory();				
					FILE_UTIL::mkdir($temp_dir);
					
					$session_dump_file = null;
					
					if(isset($_SESSION)) {					
						$session_dump = "";
						
						foreach($_SESSION as $key=>$value) 
							$session_dump .= $key." => ".@self::print_r($value,true)."\n\n";
						
						$session_dump_file = $temp_dir."session_dump.txt";
						FILE_UTIL::put_file_contents($session_dump_file,$session_dump);	
					}

					$system_name = SYSTEM_MANAGER::get_system_name() ? SYSTEM_MANAGER::get_system_name() : SYSTEM_MANAGER::get_system_host();
					
					foreach($notify_recipients as $notify_recipient)
						$email_cmodel->add_to($notify_recipient);
					
					$subject = $system_name.": ".$errmsg;

					if($error_name=get_value(self::get_error_type_list(),$errno))
						$subject = "[".$error_name."] ".$subject;
					
					$email_cmodel->enable_text();
					$email_cmodel->set_subject($subject);
					$email_cmodel->set_body(implode("\n\r",$message));
					
					if($session_dump_file)	
						$email_cmodel->add_attachment($session_dump_file);
						
					try {
						$has_success = $email_cmodel->send($notify_recipients);
					} catch(Exception $e) {
						$message[] = "Failed to send error notification: ".$e->getMessage()."\n\n";
					}
					
					FILE_UTIL::rmdir($temp_dir);
					
				} else
					$message[] = "No notification recipients to send error notification to";
			}
			
			if(!$has_success && SERVER_UTIL::is_command_application())
				echo implode("\n\r",$message);				
		}
		
		/**
		 * Displays the backtrace 
		 *
	 	 * @param int $errno integer value of the error type 
		 * @param string $errmsg details on what the error is
                 * @param string $filename 
		 * @param int $linenum line number on which the error occured
		 * @return string
	 	 */
		
		static function display_error($errno, $errmsg, $filename, $linenum) {								
			echo self::get_debugtrace($errno, $errmsg, $filename, $linenum, 2, self::$_OUTPUT_FORMAT);
		}
		
		/**
		 * Logs the error message to a flat file
		 *
		 * @param int $errno error code being checked
		 * @param string $errmsg error message
		 * @param string $filename file that the error occured
		 * @param int $linenum line number
		 * @return string
	 	 */

		static function log_error($errno, $errmsg, $filename, $linenum) {
			return SYSTEM_MANAGER::get_logger()->error(array("errmsg"=>$errmsg,"errno"=>$errno,"filename"=>$filename,"linenum"=>$linenum,"stack_trace"=>self::get_stack_trace(2)));
		}		
		
        /**
         * Error bit to array
         * @param string $value
         * @return string 
         */
		static function error_bit_to_array($value) {

			$level_names = array(	E_ERROR=>"E_ERROR", 
						E_WARNING => "E_WARNING",
						E_PARSE => 'E_PARSE', 
						E_NOTICE => 'E_NOTICE',
						E_CORE_ERROR => 'E_CORE_ERROR', 
						E_CORE_WARNING => 'E_CORE_WARNING',
						E_COMPILE_ERROR => 'E_COMPILE_ERROR', 
						E_COMPILE_WARNING => 'E_COMPILE_WARNING',
						E_USER_ERROR => 'E_USER_ERROR', 
						E_USER_WARNING => 'E_USER_WARNING',
						E_USER_NOTICE => 'E_USER_NOTICE' );
			
			if(defined("E_STRICT")) 
				$level_names[E_STRICT] = "E_STRICT";
				
			$levels = array();
			if(($value & E_ALL)==E_ALL) {
				$levels[E_ALL]= "E_ALL";
				$value &= ~E_ALL;
			}
			
			foreach($level_names as $level=>$name)
				if(($value & $level)==$level) 
					$levels[$level]=$name;
					
			return $levels;
		}
		
		/**
		 * Sets the reportable error 
		 *
		 * @param array $bit
	 	 */
		
		static function set_error_reporting($bit) {
			error_reporting($bit);			
			$bits = self::error_bit_to_array($bit);			
			self::set_reportable_errors($bits);
		}

        /**
         * Remove error reporting
         * @param string $bit
         * @todo review type 
         */
		static function remove_error_reporting($bit) {		
			error_reporting(error_reporting() ^ $bit); 
		}
	}

    /**
    * Class SHUTDOWN_CALLBACK
    *
    * @package Framework
    * @subpackage Utilities
    */
    
	class SHUTDOWN_CALLBACK {

        /**
         * Callback
         * @var string $callback
         */
		private $callback;
        
        /**
         * Constructor
         * @param string $callback 
         */
		public function __construct($callback) {
			if(is_callable($callback))
				$this->callback = $callback;
			else
				die("The shutdown callback is not callback");
		}
        
        /**
         * Call
         * @return boolean 
         */
		public function call() {

			if(!$this->callback)
				return false;

			return call_user_func($this->callback);
		}

        /**
         * Unregister 
         */
		public function unregister() {
			$this->callback = false;
		}
	}	