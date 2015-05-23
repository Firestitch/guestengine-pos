<?

    /**
    * Class OBJECT  
    *
    * @package Library
    * @subpackage Common
    */
 
	class CMODEL {

		public static function __callStatic($name,$arguments) {
			
			$class = get_called_class();

			if($name=="create") {

				if(count($arguments)) {
				   $rc = new ReflectionClass($class);
				   $object = $rc->newInstanceArgs($arguments);
				} else 
					$object = new $class();
			
				return $object;
			}

			trigger_error("Call to undefined method ".$class."::".$name."()", E_USER_ERROR);
		}


		/*
		function __clone() {
			
			foreach($this as $key => $val) {
				
				if(is_object($val) || (is_array($val))) 
					$this->{$key} = unserialize(serialize($val));

					//serialize() fails on objects that dont support serialization
			}
		}
		*/

	    public function __clone() {
	    		
	        foreach($this as $key => $val) {
	            if(is_object($val)) {

	            	$this->{$key} = clone $val;
	            	
	            } elseif(is_array($val)) {

	            	$arry = array();
	            	foreach($val as $index=>$item) {

	            		if(is_object($item)) {
 
	            			$cloned = clone $item;
	            			$cloned->hash = spl_object_hash($cloned);
	            			
	            			$this->{$key}[$index] = $cloned;
	            		} else
	            			$this->{$key}[$index] = $item;
	            	}           	
	            }	            
	        }
	    }

	    function __call($function,$args) {
	    	
	    	if($function=="clone")
	    		return clone $this;

	    	trigger_error("Call to undefined method ".get_class($this)."->".$function."()", E_USER_ERROR);
	    }	
	}	    	
