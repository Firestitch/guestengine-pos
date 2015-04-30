<?php
        /**
        * Class FILE_UTIL
        *
        * @package Framework
        * @subpackage Utilities
        */
	
	class FILE_UTIL {
		
		const DS = DIRECTORY_SEPARATOR;
		
		/**
		 * Takes a file and returns its lower case extension 
		 *
		 * <code>  
		 * echo FILE_UTIL::get_file_extension("myfile.html");
		 * </code>
		 * <pre>
		 * html
		 * </pre>
		 *
		 * @param string $filename string representation of a file
		 * @return string
		 */
		static function get_file_extension($filename) {
			return strtolower(str_replace(".","",strrchr($filename,'.')));
		}
		
		/**
		 * Takes a file and returns the entire string excluding the extension
		 *
		 * <code>  
		 * echo FILE_UTIL::remove_file_extension("myfile.html");
		 * </code>
		 * <pre>
		 * myfile
		 * </pre>
		 *
		 * @param string $str string representation of a file
		 * @return string
		 */
		static function remove_file_extension($str) {
			if(($pos=strrpos($str,"."))!==false)
				return substr($str,0,$pos);
			return $str;
		}
		
		/**
		 * Returns the folder in which the file is located
		 *
		 * <code>  
		 * echo FILE_UTIL::get_file_folder("c:\myfile.html");
		 * </code>
		 * <pre>
		 * c:\
		 * </pre>
		 *
		 * @param string $file string representation of a file
		 * @return string
		 */
		static function get_file_folder($file) {
			return dirname($file).DIRECTORY_SEPARATOR;
		}
		
		/**
		 * Returns time of the last modification to the file
		 *
		 * <code>  
		 * FILE_UTIL::get_file_modified_time("c:\myfile.html");
		 * </code>
		 *
		 * @param string $file string representation of a file
		 * @return string
		 */
		static function get_file_modified_time($file) {
			return @filemtime($file);
		}
		
		/**
		 * Takes the string file path and returns the file name
		 *
		 * <code>  
		 * echo FILE_UTIL::get_filename("c:\php.ini");
		 * </code>
		 * <pre>
		 * php.ini
		 * </pre>
		 *
		 * @param string $file_path string representation of a file path
		 * @return string
		 */
		static function get_filename($file_path) {
			return basename($file_path);
		}
		
		/**
		 * Takes a file and returns the size value
		 *
		 * <code>  
		 * FILE_UTIL::get_filesize("c:\php.ini");
		 * </code>
		 *
		 * @param string $file string representation of a file
		 * @return string
		 */
		static function get_filesize($file) {
			
			if(SERVER_UTIL::is_os_windows()) {
			
				try {
					$fsobj = new COM("Scripting.FileSystemObject");
					$file = $fsobj->GetFile($file);
					return $file->Size;
				
				} catch(Exception $e) {
					return 0;
				}
            		}
		
			return @filesize($file);
		}
		
		/**
		 * Creates a directory
		 *
		 * <code>
		 * echo FILE_UTIL::mkdir("C:\myfolder");
		 * </code>
		 * <pre>
		 * "C:\myfolder"
		 * </pre>
		 *
		 * @param string $dirname the directory created
		 * @param boolean $override overrides if there is a directory already present
		 * @param boolean $debug enables debugging
		 * @return string
		 */
		static function mkdir($dirname,$override=false,$debug=false) {
			$dirname = self::get_sanitized_directory($dirname);

			if($override)
				FILE_UTIL::rmdir($dirname,$debug);
			
			if(is_dir($dirname)) 
				return true;

			$dirs = explode(DIRECTORY_SEPARATOR,$dirname);
			$path = "";
			foreach($dirs as $dir) {
				
				$path.=$dir.DIRECTORY_SEPARATOR;
				
				if (!is_dir($path)) {
					if($debug)
						mkdir($path);
					else 
						@mkdir($path);
				}
			}
			return is_dir($dirname);
		}
		
		/**
		 * Attempts to change the owner of the file filename  to user user
		 *
		 * @param string $path path of the file to be changed
		 * @param string $uid user id 
		 * @param int $gid
		 * @param boolean $recursive
		 * @param boolean $debug enables debugging
		 */
		static function chown($path,$uid,$gid,$recursive=true,$debug=false)  {

			if(SERVER_UTIL::is_os_windows()) 
				return true;
			
			if(self::is_protected_directory($path))
				return false;
			
			$cmd = "chown -R ".$uid.":".$gid." ".$path;
			
			return SERVER_UTIL::exec($cmd);
		}
		
                /**
                 * Set group ID on execution 
                 * Reference: http://www.yolinux.com/TUTORIALS/LinuxTutorialManagingGroups.html
                 * 
                 * @param string $path
                 * @return boolean 
                 */
		static function set_group_id($path) {
			if(SERVER_UTIL::is_os_windows()) return false;

			if(self::is_protected_directory($path))
				return false;
				
			$cmd = "chmod g+s -R ".$path;
			SERVER_UTIL::exec($cmd);
		}
		
		/**
		 * Changes a directory name
		 *
		 * <code>
		 * FILE_UTIL::cd("C:\myfolder","D:\myfolder");
		 * </code>
		 *
		 * @param string $current_directory directory to be changed
		 * @param string $directory result directory
		 * @return string 
		 */
		static function cd($current_directory,$directory) {
			$current_directory = realpath($current_directory.DIRECTORY_SEPARATOR.$directory);
			return FILE_UTIL::get_sanitized_directory($current_directory);
		}
		
                /**
                 * Chmod
                 * @param string $path
                 * @param integer $bit
                 * @param boolean $recursive
                 * @param boolean $debug
                 * @return boolean 
                 */
		static function chmod($path,$bit,$recursive=true,$debug=false)  {

			if(SERVER_UTIL::is_os_windows()) return false;
			
			if(self::is_protected_directory($path))
				return false;
			
			$cmd = "chmod -R ".$bit." ".$path;
						
			return SERVER_UTIL::exec($cmd);
		}
		
		/**
		 * Removes a directory
		 *
		 * <code>
		 * FILE_UTIL::rmdir("C:\myfolder");
		 * </code>
		 *
		 * @param string $dir directory to be removed
		 * @param boolean $debug enables debugging
		 * @return boolean
		 */
		static function rmdir($dir,$debug=false) {
			
			if(self::is_protected_directory($dir))
				return false;			
			
			if(is_dir($dir)) {
				$d = @dir($dir);

				while (false !== ($entry = @$d->read())) {
					if($entry == '.' || $entry == '..') continue;

					$node = self::get_sanitized_path($dir.DIRECTORY_SEPARATOR.$entry);

					if(is_file($node)) {
						if(!self::delete($node,$debug))
							return false;
					} elseif(is_dir($node)) {
						if(!self::rmdir($node,$debug))
							return false;
					}
				}
				$d->close();
				if($debug)
					$result = rmdir($dir);
				else $result = @rmdir($dir);
				return $result;
			}
			return true;
		}
		
		/**
		 * Deletes a file
		 *
		 * <code>
		 * FILE_UTIL::delete("C:\myfolder\koala.jpg");
		 * </code>
		 *
		 * @param string $file
		 * @param boolean $debug enables debugging
		 * @return boolean
		 */
		static function delete($file,$debug=false) {
			$result = false;
			if(is_file($file)) {
				if(!$debug)
					$result = @unlink($file);
				else $result = unlink($file);
			}
			return $result;
		}
		
		/**
		 * Moves a file
		 *
		 * <code>
		 * FILE_UTIL::move("C:\myfolder\koala.jpg","D:\myfolder\koala.jpg");
		 * </code>
		 *
		 * @param string $from path of the file to be moved
		 * @param string $to path of the new file location
                 * @param boolean $override
                 * @param boolean $create_directory
                 * @param boolean $debug
		 * @return string 
		 */
		static function move($from,$to,$override=false,$create_directory=false,$debug=false) {
			
			if(self::is_protected_directory($from))
				return false;

			$from 	= rtrim($from,"/");
			$to 	= rtrim($to,"/");
				
			if($create_directory) {
				$folder = FILE_UTIL::get_file_folder($to);
				FILE_UTIL::mkdir($folder);
			}
			return FILE_UTIL::rename($from,$to,$override,$debug);
		}
		
		/**
		 * Renames a file
		 *
		 * <code>
		 * $current_directory = "C:\myfolder";
		 * $directory = "C:\mynewfolder";
		 * echo FILE_UTIL::rename($current_directory,$directory);
		 * </code>
		 * <pre>
		 * "C:\mynewfolder"
		 * </pre>
		 *
		 * @param string $from path of the old file name
		 * @param string $to path of the new file name
                 * @param boolean $override
                 * @param boolean $debug
		 * @return string 
		 */
		static function rename($from,$to,$override=false,$debug=false) {
			
			if(self::is_protected_directory($from))
				return false;
				
			if($from==$to) return false;
			if($override) {
				if(is_file($to))
					FILE_UTIL::delete($to,$debug);
				elseif(is_dir($to)) 
					FILE_UTIL::rmdir($to,$debug);
			}

			if(!$debug)
				return @rename($from,$to);
			else
				return rename($from,$to);
		}
		
		/**
		 * Copies a file
		 *
		 * <code>
		 * FILE_UTIL::copy("C:\myfolder\koala.jpg","C:\newfolder\koala.jpg");
		 * </code>
		 *
		 * @param string $file_src source file
		 * @param string $file_dest copied file
                 * @param boolean $debug
		 * @return boolean 
		 */
		static function copy($file_src,$file_dest,$debug=true) {
		
			if(!is_file($file_src))
				return false;
		
			if($debug)
				return copy($file_src,$file_dest);
			else 
				return @copy($file_src,$file_dest);
		}
		
                /**
                 * Copy directory contents
                 * 
                 * @param string $souce_directory
                 * @param string $destination_directory
                 * @param string $pattern
                 * @param boolean $dirs_only
                 * @param boolean $files_only
                 * @param boolean $overwrite
                 * @return boolean 
                 */
		static function copy_directory_contents($souce_directory,$destination_directory,$pattern="*",$dirs_only=false,$files_only=false,$overwrite=true) {
			
			$items = self::get_directory_listing($souce_directory,$pattern,false,$dirs_only,$files_only);

			foreach($items as $file) {
				$souce_file 		= $souce_directory.$file;
				$destination_file 	= $destination_directory.$file;
				
				if(is_file($souce_file))
					self::copy($souce_file,$destination_file);
				
				elseif(is_dir($souce_file))
					self::copy_directory($souce_file,$destination_file,$overwrite);				
			}
			
			return true;			
		}
		
		
		/**
		 * Returns a directory listing
		 *
		 * <code>
		 * FILE_UTIL::get_directory_listing("C:\myfolder");
		 * </code>
		 *
		 * @param string $directory directory of that the listing is coming from
		 * @param string $pattern pattern to look for
		 * @param boolean $as_index
		 * @param boolean $dirs_only
		 * @param boolean $files_only
                 * @param integer $flag_bit
		 * @return boolean 
		 */
		static function get_directory_listing($directory,$pattern="*",$as_index=false,$dirs_only=false,$files_only=false,$flag_bit=0) {
		
			$directory = self::get_sanitized_directory($directory);
			
			if($dirs_only)
				$flag_bit += GLOB_ONLYDIR;
			
			$files = @glob($directory.$pattern,$flag_bit);
			
			$dir_list = array();
			if(is_array($files)) {
			
				foreach($files as $file) {
					
					if($files_only && !is_file($file))
						continue;
				
					$filename = basename($file);
					
					if(!$as_index) 
						$dir_list[] = $filename;
					else 
						$dir_list[$filename] = $filename;
				}
			}
			
			return $dir_list;
		}
                
                /**
                 * Returns directory files
                 * @param string $dir
                 * @return array 
                 */
		public function get_directory_files($dir) {
			$dir = self::get_sanitized_directory($dir);
			return self::_get_directory_files($dir,"");
		}
                
                /**
                 * Returns directory files
                 * @param string $root_dir
                 * @param string $path
                 * @return array of objects 
                 */
		public function _get_directory_files($root_dir,$path="") {

			$files = array();

			$dir_files = self::get_directory_listing($root_dir.$path);

			$path = $path."/";
					
			foreach($dir_files as $dir_file) {

				$file = $root_dir.$path.$dir_file;

				if(is_dir($file)) 
					$files = array_merge($files,self::_get_directory_files($root_dir,$path.$dir_file));
				else {
					$size = self::get_filesize($root_dir.$path.$dir_file);
					$files[] = new BASE_CMODEL_FILE($path.$dir_file,$size);
				}
			}
			
			return $files;
		}		
		
		/**
		 * Returns a file listing
		 *
		 * <code>
		 * FILE_UTIL::get_file_listing("C:\myfolder");
		 * </code>
		 *
		 * @param string $directory directory of that the listing is coming from
		 * @param string $pattern pattern to look for
		 * @param boolean $as_index
		 * 
		 * @return boolean 
		 */

		static function get_file_listing($directory,$pattern="*",$as_index=false) {	
			return self::get_directory_listing($directory,$pattern,$as_index,false,true);
		}		
		
		/**
		 * Returns a file count
		 *
		 * <code>
		 * FILE_UTIL::get_file_count("C:\myfolder");
		 * </code>
		 *
		 * @param string $directory directory in which to count the files
		 * @param string $pattern optional pattern
		 * 
		 * @return int 
		 */
		
		static function get_file_count($directory,$pattern="*") {
			return count(@glob($directory.$pattern));		
		}
		
		/**
		 * Writes to a file
		 *
		 * @param file $file file in which to write to
		 * @param string $contents contents being wrote to the file
		 * @param string $error_message error message caught
                 * @param string $flags
		 * 
		 * @return string 
		 *
		 * FILE_USE_INCLUDE_PATH	 Search for filename in the include directory. See include_path for more information.
		 * FILE_APPEND	 		If file filename already exists, append the data to the file instead of overwriting it.
		 * LOCK_EX			 Acquire an exclusive lock on the file while proceeding to the writing.		 
		 */

		static function put($file,$contents,$flags=null) {
			
			$dirname = dirname($file);
			if($dirname=="." || !is_dir($dirname))
				throw new Exception("The directory does not exist on the file system");
			
			if(@file_put_contents($file,$contents,$flags)===false)
				throw new Exception("There was a problem trying to write the file to the file system");

			return true;
		}

		static function put_file_contents($file,$contents,&$error_message="",$flags=null) {
			
			try {
				self::put($file,$contents,$flags);
			} catch(Exception $e) {
				$error_message = $e->getMessage();
			}

			return true;
		}

		/**
		 * Returns the contents of a file
		 *
		 * @param file $file file in which to write to
		 * @param string $error_message error message caught
		 * 
		 * @return string 
		 */

		static function get($file,&$error_message=null) {
			//if(!is_file($file))
			//	$error_message = "The file does not exist on the file system.";
			
			$has_success = @file_get_contents($file);
			return $has_success;
		}

		static function get_file_contents($file,&$error_message=null) { return self::get($file,$error_message); }
		

		
		/**
		 * Returns the size of a directory
		 *
		 * <code>
		 * FILE_UTIL::get_directory_size("C:\myfolder");
		 * </code>
		 *
		 * @param string $dir directory to get size from
		 * 
		 * @return string 
		 */

		function get_directory_size($dir) {
			if(!is_dir($dir)) return 0;
			
			$size = 0;
			
			if(SERVER_UTIL::is_os_windows()) {

				$size = 0;
				$obj = new COM ('scripting.filesystemobject');

				if (is_object($obj)) {
					$ref = $obj->getfolder($dir);
					$size = $ref->size;
				}
			} else {
				
				$cmd = "du -sbc ".$dir;
				
				$size = SERVER_UTIL::shell_exec($cmd);
							
				if(preg_match("/(\d+)\s/",$size,$match))
					$size = get_value($match,1);
			}
			
			return $size;
		}
		
		/**
		 * Copies a directory to a new destination
		 *
		 * <code>
		 * FILE_UTIL::copy_directory("C:\myfolder","C:\newfolder");
		 * </code>
		 *
		 * @param string $source_dir directory to get size from
		 * @param string $dest_dir new destination for the directory to be copied to
		 * @param boolean $overwrite enables overwriting of a file
		 * @param boolean $debug enables debugging
		 * 
		 * @return string 
		 */
		
		
		static function copy_directory($source_dir, $dest_dir, $overwrite=false,$debug=false) {
			if(!is_dir($dest_dir)) 
				FILE_UTIL::mkdir($dest_dir);
			
			$handle = $debug ? opendir($source_dir) : @opendir($source_dir);
			
			if(!$handle)
				return false;
				
			$has_success = true;
			
			while(false !== ($file = @readdir($handle))) { 
				if($file == '.' || $file == '..') 
					continue;

				$path = $source_dir.DIRECTORY_SEPARATOR.$file;

				if(is_file($path)) {

					if(!is_file($dest_dir.DIRECTORY_SEPARATOR.$file) || $overwrite)
						if(!FILE_UTIL::copy($path, $dest_dir.DIRECTORY_SEPARATOR.$file,$debug))
							return false;

				} elseif(is_dir($path)) {

					if(!is_dir($dest_dir.DIRECTORY_SEPARATOR.$file))
						if(!FILE_UTIL::mkdir($dest_dir.DIRECTORY_SEPARATOR.$file,$overwrite,$debug)) 							
							return false;
							
					$has_success = self::copy_directory($path, $dest_dir.DIRECTORY_SEPARATOR.$file, $overwrite); 
				}

			}

			@closedir($handle);
					
			return $has_success;
		}	
		
		/**
		 * Returns the drive letter 
		 *
		 * <code>
		 * FILE_UTIL::get_drive_letter("C:\myfolder");
		 * </code>
		 * @param string $haystack
                 * 
		 * @return string
		 */
		
		static function get_drive_letter($haystack) {
			if(SERVER_UTIL::is_os_windows())
				return substr($haystack,0,1);
			
			return false;
		}
		
		/**
		 * Returns true if a directory is protected
		 *
                 * @param string $path
                 * 
		 * @return boolean
		 */
		
		static function is_protected_directory($path) {
			return 	!SERVER_UTIL::is_os_windows() && (	self::get_sanitized_path($path)=="/" || self::get_sanitized_path($path)=="/etc/" ||
									self::get_sanitized_path($path)=="/var/" || self::get_sanitized_path($path)=="/bin/" ||
									self::get_sanitized_path($path)=="/usr/" || self::get_sanitized_path($path)=="/lib/" ||
									self::get_sanitized_path($path)=="/tmp/" || self::get_sanitized_path($path)=="/var/" ||
									self::get_sanitized_path($path)=="/home/");			
		}
		
		/**
		 * Cleans a file name by replacing irregular characters
		 * 
		 * <code>
		 * FILE_UTIL::get_sanitized_filename("C:\myfolder\koala.jpg");
		 * </code>
		 *
                 * @param string $filename
                 * 
		 * @return string
		 */
				
		static function get_sanitized_filename($filename) {
			return preg_replace("/[\/\?*:;{}\\\]/","",$filename);
		}
		
		/**
		 * Cleans a directory by replacing irregular characters
		 * 
		 * <code>
		 * FILE_UTIL::get_sanitized_directory("C:\myfolder");
		 * </code>
                 * 
                 * @param string $directory
                 * @param string $directory_seperator
		 * @return string
		 */
		static function get_sanitized_directory($directory,$directory_seperator=DIRECTORY_SEPARATOR) {
			if(!$directory)
				return "";
                        
			return rtrim(self::get_sanitized_path($directory,$directory_seperator),$directory_seperator).$directory_seperator;
		}
		
		/**
		 * Cleans a file path by replacing irregular characters
		 *
		 * <code>
		 * FILE_UTIL::get_sanitized_path("C:\myfolder\secondfolder\myfile.tiff");
		 * </code>
		 * @param string $path
                 * @param string $directory_seperator
                 * 
		 * @return string
		 */
		static function get_sanitized_path($path,$directory_seperator=DIRECTORY_SEPARATOR) {
			$regex = $directory_seperator=="/" ? array("/\\\/","/\/{2,}/") : array("/\//",'/\\\{2,}/');
			return preg_replace($regex,array($directory_seperator,$directory_seperator),$path);
		}
		
		/**
		 * Appends a directory
		 *
		 * <code>
		 * FILE_UTIL::get_appeneded_directory("C:\myfolder","C:\appendedfolder");
		 * </code>		 
		 *
		 * @param string $dir1 string directory 
		 * @param string $dir2 string appended folder
		 * @param string $directory_seperator string separator
		 * @return string
		 */
		
		function get_appeneded_directory($dir1,$dir2,$directory_seperator=DIRECTORY_SEPARATOR) {
			return self::get_sanitized_directory($dir1).ltrim(self::get_sanitized_directory($dir2),$directory_seperator);
		}
		
		/**
                 *Returns the safe file
                 * @param string $file
                 * @return mixed 
                 */
		static function get_safe_file($file) {
			if(!SERVER_UTIL::is_os_windows())	
				$file = preg_replace('/\$/','',$file);
			return $file;
		}
		
		/**
		 * Returns a directory listing
		 *
		 * <code>
		 * FILE_UTIL::get_files("C:\myfolder");
		 * </code>
		 *
		 * @param string $directory directory of that the listing is coming from
		 * @param string $pattern pattern to look for
                 * @param boolean $recursive
		 * @param boolean $dirs_only
		 * 
		 * @return boolean 
		 */

		static function get_files($directory,$pattern="*",$recursive=true,$dirs_only=false) {
		
			$directory = self::get_sanitized_directory($directory);
			
			$flag_bit = 0;
			
			if($dirs_only)
				$flag_bit += GLOB_ONLYDIR;
			
			$files = @glob($directory.$pattern,$flag_bit);
			
			$list = array();
			if(is_array($files)) {
			
				foreach($files as $file) 
					$list[] = new FILE_ENTRY($file);
				
				foreach($files as $file) 	
					if(is_dir($file))
						$list = array_merge($list,self::get_files($file,$pattern,$dirs_only));
			}
			
			return $list;
		}	

		static function is_extension($file,$extension) 	{ return strtolower($extension)==FILE_UTIL::get_file_extension($file); }

		static function sanitize($directory,$directory_seperator=DIRECTORY_SEPARATOR) {
			if(!$directory)
				return "";

			return rtrim(self::get_sanitized_path($directory,$directory_seperator),$directory_seperator).$directory_seperator;
		}		
	}
        
        require_once("SERVER_UTIL.inc");

        /**
         * Class FILE_ENTRY
         *
         * @package Framework
         * @subpackage Utilities
         */
        
	class FILE_ENTRY {

                /**
                 * File
                 * @var string $file
                 */
		public $_file	= "";
                /**
                 * Size
                 * @var integer $size
                 */
		public $_size	= 0;
		
                /**
                 * Constructor.
                 * @param string $file the file.
                 */
		function __construct($file) {
			$this->_file = $file;
			$this->_size = @filesize($file);			
		}
		
                /**
                 * Return the file
                 * @return string 
                 */
		function get_file() 		{ return $this->_file; }
                
                /**
                 * Returns the file name
                 * @return string
                 */
		function get_file_name() 	{ return basename($this->_file); }
                
                /**
                 * return the formatted size
                 * @return string
                 */
		function get_formatted_size() 	{ return FORMAT_UTIL::get_formatted_filesize($this->_size); }
                
                /**
                 * Checks whether is directory
                 * @return boolean 
                 */
		function is_directory() 	{ return is_dir($this->get_file()); }
                
                /**
                 * Returns the extension of the file
                 * @return string 
                 */
		function get_extension() 	{ return FILE_UTIL::get_file_extension($this->get_file()); }
               
                /**
                 * Returns the contents of the file
                 * @return string 
                 */
		function get_contents() 	{ return FILE_UTIL::get_file_contents($this->_file); }
	}	