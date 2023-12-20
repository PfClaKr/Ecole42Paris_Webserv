<html>
	<body>
	<?php
			if (getenv("REQUEST_METHOD") == "POST") 
			{
				if (getenv("STATUS_UPLOAD")=='0')
				{
					echo (getenv("FILENAME"));
					echo(" Uploaded Successfully");
				}
				else
					echo("Error uploading file");
			}
		?>
        <h4><a href="/">..</a><br><h4>
	</body>
</html>
