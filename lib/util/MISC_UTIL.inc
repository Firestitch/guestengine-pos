<?php
        /**
        * Class MISC_UTIL
        *
        * @package Framework
        * @subpackage Utilities
        */

	class MISC_UTIL {
	
		/**
		 * Preforms a check to see if the string is empty
		 *
		 * <code>
		 * echo MISC_UTIL::is_empty($myname);
		 * </code>
		 *
		 * @param string $value string checked 
		 * @return boolean
	 	 */

		static function is_empty($value) {
			if(is_string($value))
				$value = trim($value);
			return empty($value);
		}

		/**
		 * Returns a random string
		 *
		 * <code>
		 * echo MISC_UTIL::get_guid();
		 * </code>
		 * <pre>
		 * b115eaf5267a3fea00a58634e6510641
		 * </pre>
		 *
		 * @param int $length int length 
                 * @param boolean $mixed_case
		 * @return string
	 	 */
	 	 
		static function get_guid($length=null,$mixed_case=false) {
			
			//generate a random id encrypt it and store it in $guid 
			$guid = md5(crypt(uniqid(rand(),true))); 

			if($length)	
				$guid = substr($guid,0,$length);
			
			if($mixed_case) {
				
				$chars = str_split($guid);
				$guid = "";
				
				foreach($chars as $char) 
					$guid .= rand(0,1) ? strtoupper($char) : $char;
			}	
			
			return $guid;
		}

		/**
		 * Returns a random string
		 *
		 * <code>
		 * echo MISC_UTIL::get_random_uid();
		 * </code>
		 * <pre>
		 * b115eaf5267a3fea00a58634e6510641
		 * </pre>
		 *
		 * @return string
	 	 */
	 	 
		static function get_random_uid() {
			return self::get_guid();
		}		
		
		/**
		 * Merges to arrayss
		 *
		 * <code>
		 * $words = array("1"=>"cat","2"=>"dog","3"=>"mouse");
		 * $words2 = array("4"=>"horse","5"=>"rabbit","6"=>"eagle");
		 * echo MISC_UTIL::array_merge($words,$words2)
		 * </code>
		 * <pre>
		 * Array
		 * (
	         *	[1] => cat
    		 *	[2] => dog
    		 *	[3] => mouse
      		 *	[4] => horse
    		 *	[5] => rabbit
    		 *	[6] => eagle
		 * )
		 * </pre>
		 *
		 * @param array $array1 array first list
		 * @param array $array2 array second list 
		 * @return array
	 	 */
	 	 
		static function array_merge($array1,$array2) {
			foreach($array2 as $key=>$value)
				$array1[$key] = $value;
			return $array1;
		}

		public static function casts($old_objects,$new_classname) {

			foreach($old_objects as &$old_object)
				$old_object = self::cast($old_object,$new_classname);

			return $old_objects;
		}

	}

	/**
	 * Prints output to the screen
	 *
	 * <code>
	 * p(array(1=>"hello"));
	 * </code>
	 * <pre>
	 * Array
	 * (
	 *   [1] => hello
	 * )
	 * </pre>
         * 
	 * @return boolean
	 */

	function p() {

		$backtrace = debug_backtrace();

		if($debug=get_value($backtrace,0)) {

			$if_format_html = false;
			
			if($if_format_html)
				echo '<div style="padding:2px;border: 0px dashed red;font-weight:bold">DEBUG: '.get_value($debug,"file").' @ line number '.get_value($debug,"line");
			else 
				echo 'DEBUG: '.get_value($debug,"file").' @ line number '.get_value($debug,"line")."\n\n";

			foreach(func_get_args() as $value) {

				if($value===null) 
					$value = "NULL";
				elseif($value===FALSE) 
					$value = "FALSE";
				elseif($value===TRUE) 
					$value = "TRUE";
				
				if(is_array($value) || is_object($value)) {

					if($if_format_html)
						print "<pre style='margin:4px;margin-left:10px;font-weight:normal'>";	

					print_r($value);

					if($if_format_html)
						print "</pre>";
				} else {

					if($if_format_html)
						print "<pre style='margin:4px;margin-left:10px;font-weight:normal'>";					

					if(is_string($value) && $if_format_html)
						$value = htmlentities($value);
					
					print($value);

					if($if_format_html)
						print "</pre>";
					else 
						print "\n\n";
				}
			}

			if($if_format_html)
				echo '</div>';
		}
	}
	
	/**
         * H 
         */
	function h() {
		$backtrace = debug_backtrace();

		if($debug=get_value($backtrace,0)) {		
				
			$values = array();
				
			foreach(func_get_args() as $value) {

				if($value===null) $value = "NULL";
				elseif($value===FALSE) $value = "FALSE";
				elseif($value===TRUE)  $value = "TRUE";
				else 
					$value = print_r($value,true);
				
				$values[] = $value;
			}
			
			$value = count($values)>1 ? print_r($values,true) : get_value($values,0);
			
			$value = 'DEBUG: '.get_value($debug,"file").' @ line number '.get_value($debug,"line")." ".$value;
		
			echo HTML_UTIL::get_hidden("debug",$value);
		}
	}
	
	/**
	 * Preforms a check to see if a string is empty
	 *
	 * <code>
	 * echo is_empty("mystring");
	 * </code>
	 * <pre>
	 * false
	 * </pre>
	 *
	 * @param string $str string checked 
	 * @return boolean
	 */
	 
	function is_empty($str) { return MISC_UTIL::is_empty($str); }

	/**
	 * Returns the value of a given index
	 *
	 * <code>
	 * Ex 1.
	 * $words = array("1"=>"cat","2"=>"dog","3"=>"mouse");
	 * echo get_value($words,"2");
	 * </code>
	 * <pre>
	 * dog
	 * </pre>
	 * <code>
	 * Ex 2.
	 * $animals = array(1=>array(3=>"dog"),2=>array(4=>"cat"));
	 * echo $animals,get_value($animals,array(2,4));
	 * </code>
	 * <pre>
	 * cat
	 * </pre>
	 *
	 * @param array $var array 
	 * @param mixed $index or path of indexes 
	 * @param mixed $default
	 * @return mixed
         * @todo review return type
	 */
	function value($var,$index,$default=null) {
		return get_value($var,$index,$default);	
	}

	function get_value($var,$index,$default=null) {
	
		if($var===null)
			return $default;
		
		$is_array = is_array($index);
		
		if($is_array && count($index)==1) {
			$index = array_shift($index);
			$is_array = is_array($index);
		}
			
		if($is_array) {

			if(($first_index=get_value($index,0,null))!==null) {

				if(($var=get_value($var,$first_index,null))!==null) {

					array_shift($index);

					$index = count($index)==1 ? $index[0] : $index;

					if(!is_array($var))
						return $default;

					return get_value($var,$index,$default);
				}
			}

			return $default;

                } elseif(is_array($var)) {
                        if(@array_key_exists($index,$var))
                                return $var[$index];
                }
					
		return $default;
	}

	/**
	 * Returns the numeric value
	 *
	 * <code>
	 * Ex 1.
	 * $numbers = array("1"=>"42","2"=>"56","3"=>"89");
	 * echo get_numeric_value($numbers,"2");
	 * </code>
	 * <pre>
	 * 56
	 * </pre>
	 *
	 * @param array $var array 
	 * @param mixed $index or path of indexes 
	 * @param mixed $default
	 * @return boolean
         * @todo review return type - it seems wrong
	 */
	 
	function get_numeric_value($var,$index,$default=null) {
		$value = get_value($var,$index,$default);		
		return is_numeric($value) ? $value : null;
	}


