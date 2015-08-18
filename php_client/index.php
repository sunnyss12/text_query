<!doctype html>
<html lang="en">
<head>
    <meta charset="utf-8">
    <title>Search</title>
  <!--  <script type="text/javascript" src="http://ajax.googleapis.com/ajax/libs/jquery/1.4.0/jquery.min.js"></script> -->
  <script src="//code.jquery.com/jquery-1.10.2.js"></script>
    <link rel="stylesheet" href="//code.jquery.com/ui/1.11.4/themes/smoothness/jquery-ui.css">
    <script src="//code.jquery.com/ui/1.11.4/jquery-ui.js"></script>
    <style type="text/css">
        #container{  
          min-height:100%;  
          position:relative;  
          text-align:center;
         } 
 
         #content{ 
            padding:10px;  
            padding-bottom:550px;  
         }  

         #footer{  
              position:absolute;  
              bottom:0;  
              padding:10px0;   
              width:100%;  
         }  
        body{
        //    text-align: center;
        }
        input{
            height: 35px;
            width: 500px;
            font-size:20px;
            line-height:30px;
            border-color: red;
        }
        button{
            height: 40px;
            width: 70px;
            background-color: GreenYellow;
            border-color:GreenYellow; 
        }
        #search-field{
             position: relative; 
             top: 50px;
        }
    </style>

<script>
$(document).ready(
    function () {
            $("#inputtext").autocomplete({
            source: "test.php"
        });
    });

</script>

</head>
<body background="15.gif">
<div id="container">
    <div id="content">
        <div id="search-field" class="ui-widget">
                <input type="text" id="inputtext" name="input"/>
                <button type="submit" id="idsub" >搜索</button>
                <div id="result"></div>
        </div>
    </div>
    <div id="footer">
<script>
document.write(Date());
</script>
    </div>

</div>
</body>
</html>
