<?
    /**
    * Class BASE_CMODEL_FORMAT
    *
    * @package Framework
    * @subpackage Cmodels
    */

	class CMODEL_FORMAT {

        /**
         * The value
         * @var mixed $value 
         */
        private $_value     = null;
        private $_options   = array();
       
        /**
         * Constructor
         * @param integer $time 
         */
        public function __construct($value,$options=array()) {
            $this->_value = $value;
            $this->_options = $options;
        }

        public static function create($value,$options=array()) {
            return new CMODEL_FORMAT($value,$options);
        } 

        public function value() { return $this->_value; }

        public function medium_date() {
            return DATE_UTIL::get_medium_date($this->value());
        }

        public function medium_date_time($options=array()) {

            $format = "%B %e%0S, %Y %0g:%M %0a";

            if(value($options,"seconds"))
                $format .= " %S seconds";

            return $this->format_time($format,$options);
        }

        public function time($options=array("seconds"=>false)) {

            $format = "%0g:%M %0a";

            if(value($options,"seconds"))
                $format .= " %S seconds";

            return $this->format_time($format);
        }

        public function long_date_time($options=array("seconds"=>false,"timezone"=>null)) {

            $format = "%A %B %e%0S, %Y %0g:%M %0a";

            if(value($options,"seconds"))
                $format .= " %S seconds";

            return $this->format_time($format,$options);
        }

        public function short_date_time($options=array("seconds"=>false)) {
            return $this->short_date(array("time"=>true,"seconds"=>value($options,"seconds")));
        }

        public function yyyymmdd($options=array()) {
            return $this->format_time("%Y-%m-%d");
        }

        public function short_date($options=array()) {

            $options = $options + array("month"=>true,
                                        "day"=>true,
                                        "year"=>true,
                                        "time"=>false,
                                        "seconds"=>false);

            $parts = array();
            if(value($options,"month"))
                $parts[] = "%m";

            if(value($options,"day"))
                $parts[] = "%d";

            if(value($options,"year"))
                $parts[] = "%y";

            $format = implode("/",$parts);

            if(value($options,"time")) {
                $format .= " %0g:%M";

                if(value($options,"seconds"))
                    $format .= ":%S";

                $format .= " %0a";
            }

            return $this->format_time($format);
        }

        public function filesize() {
            return FORMAT_UTIL::get_formatted_filesize($this->value());
        }

        public function currency($round=2) {
            return FORMAT_UTIL::get_currency($this->value(),$round);
        }

        public function iso8601($options=array("time"=>true,"timezone"=>true)) {

            $parts = array();
            $parts[] = "%Y-%m-%d";

            if(value($options,"time",true))
                $parts[] = "T%H:%M:%S";

            if($timezone=value($options,"timezone",true))
                $parts[] = $timezone===true ? date("P") : $timezone;

            return $this->format_time(implode("",$parts),$this->value());
        } 

        public function ordinal() {
            return FORMAT_UTIL::get_ordinal($this->value());
        } 

        public function phone() {
            return FORMAT_UTIL::get_phone($this->value());
        } 

        public function age($abr=false,$suffix=true,$round=true) {
            return BCMODEL_TIME_AGE::create($this->_value)->age($abr,$suffix,$round);
        }

        /*
        Day --- ---
        %a  An abbreviated textual representation of the day    Sun through Sat
        %A  A full textual representation of the day    Sunday through Saturday
        %d  Two-digit day of the month (with leading zeros) 01 to 31
        %e   Day of the month, with a space preceding single digits. Not implemented as described on Windows. See below for more information.    1 to 31
        %j  Day of the year, 3 digits with leading zeros    001 to 366
        %u  ISO-8601 numeric representation of the day of the week  1 (for Monday) though 7 (for Sunday)
        %w  Numeric representation of the day of the week   0 (for Sunday) through 6 (for Saturday)
        Week    --- ---
        %U  Week number of the given year, starting with the first Sunday as the first week 13 (for the 13th full week of the year)
        %V  ISO-8601:1988 week number of the given year, starting with the first week of the year with at least 4 weekdays, with Monday being the start of the week 01 through 53 (where 53 accounts for an overlapping week)
        %W  A numeric representation of the week of the year, starting with the first Monday as the first week  46 (for the 46th week of the year beginning with a Monday)
        Month   --- ---
        %b  Abbreviated month name, based on the locale Jan through Dec
        %B  Full month name, based on the locale    January through December
        %h  Abbreviated month name, based on the locale (an alias of %b)    Jan through Dec
        %m  Two digit representation of the month   01 (for January) through 12 (for December)
        Year    --- ---
        %C  Two digit representation of the century (year divided by 100, truncated to an integer)  19 for the 20th Century
        %g  Two digit representation of the year going by ISO-8601:1988 standards (see %V)  Example: 09 for the week of January 6, 2009
        %G  The full four-digit version of %g   Example: 2008 for the week of January 3, 2009
        %y  Two digit representation of the year    Example: 09 for 2009, 79 for 1979
        %Y  Four digit representation for the year  Example: 2038
        Time    --- ---
        %H  Two digit representation of the hour in 24-hour format  00 through 23
        %I  Two digit representation of the hour in 12-hour format  01 through 12
        %l (lower-case 'L') Hour in 12-hour format, with a space preceeding single digits    1 through 12
        %M  Two digit representation of the minute  00 through 59
        %p  UPPER-CASE 'AM' or 'PM' based on the given time Example: AM for 00:31, PM for 22:23
        %P  lower-case 'am' or 'pm' based on the given time Example: am for 00:31, pm for 22:23
        %r  Same as "%I:%M:%S %p"   Example: 09:34:17 PM for 21:34:17
        %R  Same as "%H:%M" Example: 00:35 for 12:35 AM, 16:44 for 4:44 PM
        %S  Two digit representation of the second  00 through 59
        %T  Same as "%H:%M:%S"  Example: 21:34:17 for 09:34:17 PM
        %X  Preferred time representation based on locale, without the date Example: 03:59:16 or 15:59:16
        %z  Either the time zone offset from UTC or the abbreviation (depends on operating system)  Example: -0500 or EST for Eastern Time
        %Z  The time zone offset/abbreviation option NOT given by %z (depends on operating system)  Example: -0500 or EST for Eastern Time
        Time and Date Stamps    --- ---
        %c  Preferred date and time stamp based on local    Example: Tue Feb 5 00:45:10 2009 for February 5, 2009 at 12:45:10 AM
        %D  Same as "%m/%d/%y"  Example: 02/05/09 for February 5, 2009
        %F  Same as "%Y-%m-%d" (commonly used in database datestamps)   Example: 2009-02-05 for February 5, 2009
        %s  Unix Epoch Time timestamp (same as the time() function) Example: 305815200 for September 10, 1979 08:40:00 AM
        %x  Preferred date representation based on locale, without the time Example: 02/05/09 for February 5, 2009
        Miscellaneous   --- ---
        %n  A newline character ("\n")  ---
        %t  A Tab character ("\t")  ---
        %%  A literal percentage character ("%")    ---
        */
        public function format_time($format,$options=array()) {

            $time = $this->value();
            
            if(!$time) return "";

            $time = $time==-1 ? time() : $time;

            $time = !is_numeric($time) ? strtotime($time) : $time;

            $new_timezone = value($options,"timezone");

            if(!$new_timezone)
                $new_timezone = value($this->_options,"timezone");

            if($new_timezone) {
                $system_timezone = date_default_timezone_get();
                date_default_timezone_set($new_timezone);
            }           

            $format = str_replace("%O",date("S", $time),$format);

            $mapping["%e"]  = date("j", $time);
            $mapping["%0S"] = date("S", $time);
            $mapping["%0a"] = date("a", $time);
            $mapping["%0g"] = date("g", $time);

            $format = str_replace(array_keys($mapping),array_values($mapping),$format);

            $formatted = strftime($format,$time);

            if($new_timezone)
                date_default_timezone_set($system_timezone);

            return $formatted;
        }

        public function percent($round=0) {
            return round($this->value(),$round)."%";
        }

        public function round($limits=array()) {
                
            foreach($limits as $value=>$round)
                if((float)$this->value()<(float)$value)
                    return round($this->value(),$round);

            return round($this->value());
        }
	}