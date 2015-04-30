<?
	class CMODEL_MAITRED_PROCESSOR extends CMODEL_PROCESSOR {
	
		public function build() {
			
			$stores = FILE_UTIL::get_directory_listing($this->data_dir());

			foreach($stores as $store) {

				$store_dir = $this->data_dir().$store."/DATA/";

				$xml_files = FILE_UTIL::get_file_listing($store_dir,"*.xml");
			
				foreach($xml_files as $xml_file)
					if(preg_match("/^invoice.*.xml/",$xml_file)) 
						$this->add_file($store_dir.$xml_file,$store."/".$xml_file,$store);
			}
		}
	}