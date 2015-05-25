<?
	class CMODEL_SQLSRV extends CMODEL {

		protected $_debug = false;

		public function connect() {			

			if(!extension_loaded("sqlsrv"))
				throw new Exception("The sqlsrv extension does not exist");				

			$serverName = "(local)";
			$connectionInfo = array( "Database"=>"Squirrel", "UID"=>"sa", "PWD"=>"Squirrel2007");
			$this->_conn = sqlsrv_connect($serverName, $connectionInfo);

			if( !$this->_conn ) 
				throw new Exception("Connection could not be established to ");

			return $this;
		}

		function debug() {
			$this->_debug = true;
			return $this;
		}

		function select($sql) {

			if($this->_debug) {
				p($sql);
				flush();
			}
						   
			$stmt = sqlsrv_query( $this->_conn, $sql );
			if( $stmt === false) {
				p(sqlsrv_errors());
			    throw new Exception(value(sqlsrv_errors(),"message"));
			}

			$rows = array();
			while( $row = sqlsrv_fetch_array( $stmt, SQLSRV_FETCH_ASSOC) ) {

			    foreach($row as &$col) 
			      	if(is_string($col))
			      		$col = trim($col);
			    
			    $rows[] = $row;
			}

			return $rows;
		}
	}