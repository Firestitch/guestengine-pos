<?
	class CMODEL_MAITRED_PROCESSOR extends CMODEL_PROCESSOR {
	
		public function get_files() {
			
			$stores = FILE_UTIL::get_directory_listing($this->data_dir());

			$files = array();
			foreach($stores as $store) {

				$store_dir = $this->data_dir().$store."/DATA/";

				$xml_files = FILE_UTIL::get_file_listing($store_dir,"*.xml");
			
				foreach($xml_files as $filename) {
					if(preg_match("/^invoice.*.xml/",$filename)) {
						$files[] = array(	"file"=>$store_dir.$filename,
											"filename"=>$filename,
											"path"=>$store."/".$filename,
											"store"=>$store);
					}
				}
			}

			return $files;
		}
	}