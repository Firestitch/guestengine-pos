<?
    /**
    * Class BASE_CMODEL_TIME
    *
    * @package Framework
    * @subpackage Cmodels
    * CMODEL_TIME::create("2014-02-02")->diff_days(time()); //return difference in days
    * CMODEL_TIME::create(time())->round_day(); //removes the time from the timestamp
    * CMODEL_TIME::create(time())->add_day(8)->db(); //add 8 days and return DB date/time
    * CMODEL_TIME::create(time())->age(); //returns "2 days ago"
    */

	class CMODEL_TIME {

        const SECONDS_DAY = CONSTANT::SECONDS_DAY;

        /**
         * The time
         * @var integer $time
         */
		private $_time      = 0;
        private $_options   = array();

        /**
         * Constructor
         * @param integer $time
         */
		public function __construct($time,$options=array("locale"=>null,"timezone"=>null)) {

            $this->_options = $options;

			if(is_numeric($time))
				$this->_time = $time;

			else {
                $time = str_replace("T"," ",$time);
                $this->_time = strtotime($time);
            }
		}

        public static function create($time,$options=array("locale"=>null)) {
            return new CMODEL_TIME($time,$options);
        }

        function get_time()         { return $this->_time; }
        function set_time($time)    { $this->_time = $time; return $this; }

        /**
         * Return time of day seconds
         * @return integer
         */
		function get_time_of_day_seconds() { //TODO: deprecated do not use
			return (date("H",$this->_time) * 60 * 60) + (date("i",$this->_time) * 60) + date("s",$this->_time);
		}

        /**
         * Return date seconds
         * @return integer
         */
		function get_date_seconds() { //TODO: deprecated do not use
			return mktime(0,0,0,date("n",$this->_time),date("j",$this->_time),date("Y",$this->_time));
		}

        /**
         * Checks whether time is today
         * @return boolean
         */
		function is_today() {
			return ($this->_time >= mktime(0,0,0,date("n"),date("j"),date("Y"))) && ($this->_time < mktime(0,0,0,date("n"),date("j") + 1,date("Y")));
		}

        function valid() { return is_numeric($this->_time); }
        

        /**
         * Checks whether time is yesterday
         * @return boolean
         */
		function is_yesterday() {
			return ($this->_time >= mktime(0,0,0,date("n"),date("j") - 1,date("Y"))) && ($this->_time < mktime(0,0,0,date("n"),date("j"),date("Y")));
		}

        /**
         * Checks whether time is tomorrow
         * @return boolean
         */
        function is_tomorrow() {
            return ($this->_time >= mktime(0,0,0,date("n"),date("j") + 1,date("Y"))) && ($this->_time < mktime(0,0,0,date("n"),date("j") + 2,date("Y")));
        }

        function add_year($years=1) {
            $this->_time = mktime($this->hour(),$this->minute(),$this->second(),$this->month(),$this->day(),$this->year() + $years); return $this;
        }

        function add_month($months=1) {
            $this->_time = mktime($this->hour(),$this->minute(),$this->second(),$this->month() + $months,$this->day(),$this->year()); return $this;
        }

        function add_day($days=1) {
            $this->_time = mktime($this->hour(),$this->minute(),$this->second(),$this->month(),$this->day() + $days,$this->year()); return $this;
        }

        function add_minute($minutes=1) {
            $this->_time = mktime($this->hour(),$this->minute() + $minutes,$this->second(),$this->month(),$this->day(),$this->year()); return $this;
        }

        function add_second($seconds=1) {
            $this->_time = mktime($this->hour(),$this->minute(),$this->second() + $seconds,$this->month(),$this->day(),$this->year()); return $this;
        }

        function add_hour($hours=1) {
            $this->_time = mktime($this->hour() + $hours,$this->minute(),$this->second(),$this->month(),$this->day(),$this->year()); return $this;
        }

        function date($hours=1) {
            return mktime(0,0,0,$this->month(),$this->day(),$this->year());
        }

        function set_day($day) {
            $this->_time = mktime($this->hour(),$this->minute(),$this->second(),$this->month(),$day,$this->year()); return $this;
        }

        function set_month($month) {
            $this->_time = mktime($this->hour(),$this->minute(),$this->second(),$month,$this->day(),$this->year()); return $this;
        }

        function set_year($year) {
            $this->_time = mktime($this->hour(),$this->minute(),$this->second(),$this->get_month(),$this->day(),$year); return $this;
        }

        function add_week($weeks=1)             { return $this->add_day($weeks * 7); }
        
        function subtract_day($days=1)          { return $this->add_day($days * -1); }
        function subtract_week($weeks=1)        { return $this->subtract_day($weeks * 7); }
        function subtract_minute($minutes=1)    { return $this->add_minute($minutes * -1); }
        function subtract_hour($hours=1)        { return $this->add_hour($hours * -1); }
        function subtract_second($seconds=1)    { return $this->add_second($seconds * -1); }
        function subtract_month($months=1)      { return $this->add_month($months * -1); }

        function round_day()                    { return $this->set_time($this->date()); }
        function round_second()                 { return $this->set_time(mktime($this->hour(),$this->minute(),0,$this->month(),$this->day(),$this->year())); }

        function time()                         { return $this->_time; }
        function second()                       { return (int)date("s",$this->_time); }
        function minute()                       { return (int)date("i",$this->_time); }
        function hour()                         { return date("G",$this->_time); }
        function month()                        { return date("n",$this->_time); }
        function year()                         { return date("Y",$this->_time); }
        function day()                          { return date("j",$this->_time); }
        function week()                         { return date("W",$this->_time); }
        function weekday()                      { return date("w",$this->_time); }
        function gmt()                          { return date("P",$this->_time); }
        function timezone_abr()                 { return date("T",$this->_time); }
        function short_date()                   { return date("Y-m-d",$this->_time); }
        function long_date()                    { return FORMAT_UTIL::get_long_date($this->_time); }
        function long_datetime()                { return FORMAT_UTIL::get_long_date_time($this->_time); }
        function short_datetime()               { return date("%Y-%m-%d %H:%M:%S",$this->_time); }
        function age()                          { return $this->time_age()->age(); }        
        function db()                           { return $this->strftime("%Y-%m-%d %H:%M:%S"); }
        function db_date()                      { return $this->strftime("%Y-%m-%d"); }
        function db_time()                      { return $this->strftime("%H:%M:%S"); }        
        function month_name()                   { return $this->strftime("%B"); }
        function month_abr()                    { return $this->strftime("%b"); }
        function format()                       { return CMODEL_FORMAT::create($this->_time); }
        function time_age()                     { return BCMODEL_TIME_AGE::create($this->get_time()); }
        function cloner()                       { return clone $this; }

        function strftime($format,$time=null) {

            $time = $time ? $time : $this->_time;

            $locale = value($this->_options,"locale");
            $timezone = value($this->_options,"timezone");

            $system_timezone = date_default_timezone_get();

            if($timezone)
                date_default_timezone_set($timezone);

            $old_locale = null;
            if($locale) {
                if(preg_match("/^fr-/",$locale))
                    $locale = "french";

                $old_locale = setlocale(LC_ALL,"0");
                setlocale(LC_TIME,$locale);
            }

            $value = utf8_encode(strftime($format,$time)); 
            
            if($old_locale)
                setlocale(LC_TIME,$locale);

            date_default_timezone_set($system_timezone);

            return $value;
        }
        
        function daytime()                      { return $this->second() + ($this->minute() * 60) + ($this->hour() * CONSTANT::SECONDS_HOUR); }
        function daytime_minutes()              { return $this->minute() + $this->hour() * 60; }

        function years()                        { return (float)$this->diff(time(),"%a")/365.25; }
        function hours()                        { return (time()-$this->time())/3600; }
        function minutes()                      { return (time()-$this->time())/60; }

        function diff_days($time)               { return $this->diff($time,"%a"); }

        //Reference http://en.wikipedia.org/wiki/ISO_8601
        function ios8601($options=array()) { return $this->iso8601($options); }

        function iso8601($options=array()) {
            return CMODEL_FORMAT::create($this->time())->iso8601($options);
        }

        function diff($time,$format) {

            $t1 = new DateTime();
            $t1->setTimestamp($this->_time);

            $t2 = new DateTime();
            $t2->setTimestamp($time);

            return $t1->diff($t2)->format($format);
        }

        function greater($time)                 { return $this->time()>$time; }
        function lesser($time)                  { return $this->time()<$time; }

        function greaterequal($time)            { return $this->time()>=$time; }
        function lesserequal($time)             { return $this->time()<=$time; }

        public function __toString() {
            return $this->time().": ".$this->long_datetime()." ".$this->timezone_abr()." ".$this->gmt();
        }
    }