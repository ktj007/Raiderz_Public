   <?php
   require_once 'DB.php'; //Pear
   require_once 'common.php';
   
   class mydb
   {
      var $db;
      var $debug=true;
       function mydb($user='dev', $pass='dev', $server='SH_DB', $name='SoulHuntDB')
     {
          $dsn = "mssql://$user:$pass@$server/$name";
          $this->db = DB::connect($dsn);
  
          if(DB::isError($this->db)) die("DB Connect Error<br>죄송합니다. 잠시만 기다려주세요.");
      }
  
      function close()
      {
          if($this->db)
          {
              $this->db->disconnect();
          }
      }
  
      //데이타 가져오기
      function select($dbname, $arr='', $where='')
      {
         if($arr)
         {
             $sel=join(", ", $arr);
         }
         else
         {
             $sel=" * ";
         }
         $sql="select $sel from $dbname $where";
         $data=$this->q($sql);
         if($data && count($data) == 1)
         {
             return $data[0];
         }
         else
         {
            return $data;
        }
     }

    //일반퀴리
    function q($sql)
    {
        $data=$this->db->query($sql);
        $this->error($data);

        while ($row = $data->fetchRow(DB_FETCHMODE_ASSOC))
        {
            $var[] = $row;
        }

        return $var;
    }
    //퀴리 한줄만 얻기
    function ql($sql)
    {
        $data=$this->q($sql);
        if(!$data) return false;
        $data2=$data[0];
        return $data2;
    }

    //저장용 퀴리
    function sq($sql)
    {
        $data=$this->db->query($sql);
        $this->error($data);
        return $data;
    }

    //한개의 값만 얻어오기
    function qo($sql)
    {
        $data=$this->db->getOne($sql);
        $this->error($data);
        return $data;
    }

    //데이타 넣기
    function in($dbname, $arr)
    {
        $num=$this->db->autoExecute($dbname, $arr, DB_AUTOQUERY_INSERT);
        $this->error($num);
        return $num;
    }

    //데이타 업데이트
    function up($dbname, $arr, $where)
    {
        $num=$this->db->autoExecute($dbname, $arr, DB_AUTOQUERY_UPDATE, $where);
        $this->error($num);
        return $num;
    }

    //데이타 수정하기 up=up+1 이 되게
    function up2($dbname, $arr, $where='')
    {
        foreach($arr as $key=>$val)
        {
            if( (!find_str($val, "+") && !find_str($val, "-") && !find_str($val, "*") ) && is_string($val) )
            {
                $val="'".$val."'";
            }
            $filed[]="$key = $val";
        }
        $view=join(",", $filed);
        if($where) $where=" where " . $where;
        $sql="update $dbname set $view $where";    //sql문 생성
        $data=$this->sq($sql);

        return $data;
    }

    //auto_increment 현재값
    function seq($dbname, $field)
    {

        $sql="select max($field) from $dbname ";
        $val=$this->qo($sql)+0;
        return $val;
    }

    //최근 auto_increment 값
    function auto()
    {
        $sql="select LAST_INSERT_ID()";
        $val=$this->qo($sql);

        return $val;
    }

    //에러 첵킹
    function error($info)
    {
        if (DB::isError($info))
        {
            if($this->debug)
            {
                die($info->getDebugInfo());
            }
            else
            {
                die("DB 에러");
            }
        }
    }

 }

 ?>