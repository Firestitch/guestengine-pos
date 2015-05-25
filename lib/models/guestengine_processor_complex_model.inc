<?
	class CMODEL_GUESTENGINE_PROCESSOR extends CMODEL_PROCESSOR {

		public function build() {

			$stores = FILE_UTIL::get_directory_listing($this->data_dir());

			foreach($stores as $store) {

				$store_dir = $this->data_dir().$store."/";

				$files = FILE_UTIL::get_file_listing($store_dir,"*.xml");

				foreach($files as $file)
					$this->add_file($store_dir.$file,$store."/".$file,$store);
			}
		}
	}