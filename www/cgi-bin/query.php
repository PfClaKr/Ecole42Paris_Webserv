<!DOCTYPE html>
<html>
    <head>
        <meta http-equiv="content-type" content="text/html; charset=UTF-8">
        <title>Hello!</title>
    </head>
    <body>
            <h1>WE DID IT!<br></h1>
            <h2>YchunSchae's engine<br></h2>

            </form>
            <form action="/test/" method="get">
              <input type="submit" value="autoindex">
            </form>

            <h3>[TEST GET METHOD]</h3>
            <a href="/images">Test GET images</a><br>
            <a href="/a">Test GET non existing directory</a><br>
            <a href="/ychunschae/index.html">Test GET html file</a><br>

            <h3>[PHP-CGI TEST]</h3>
            <form action="query.php">
            <label for="fname">Enter Your Name:</label>
            <input type="text" id="fname" name="fname"><br>
            <input type="submit" value="submit">
			<?php
				echo "Welcome, ";
				echo $_GET["fname"];
				echo "!";
				?><br>
                
            <h3><br>[TEST POST METHOD]</h3>
            </form>
            Test Post File <br>
            <form method="post" enctype="multipart/form-data" action="upload.php">
            <input type="file" id="myid" name="Yofile"/>
            <button>submit</button>

            </form><br>

            <div>Redirection to intra</div>
            <form action="/redirection/42-intra" method="get">
              <input type="submit" value="redirection to intra">
            </form><br>
            <div>Redirection to nginx</div>
            <form action="/redirection/nginx/" method="get">
              <input type="submit" value="redirection to nginx">
            </form><br>
            </form>
            <div>Redirection to google</div>
            <form action="/redirection/google" method="get">
            <input type="submit" value="redirection to google">
            </form><br>
    </body>
</html>
