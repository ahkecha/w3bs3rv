<?php
   ob_start();
   session_start();
   $id = false;
?>
	<meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <link rel="stylesheet" href="https://stackpath.bootstrapcdn.com/bootstrap/5.0.0-alpha1/css/bootstrap.min.css" integrity="sha384-r4NyP46KrjDleawBgD5tp8Y7UzmLA05oM1iAEQ17CSuDqnUK2+k9luXQOfXJCJ4I" crossorigin="anonymous">
   </head>

   <body class="d-flex text-center">
		<?php
			if(isset($_SESSION['username']) && !empty($_SESSION['username'])) {
				echo '<div class="grid align-items-center justify-content-center m-auto">';
				echo '<h1 class="mb-3 font-weight-normal row justify-content-center">Hello ';
				echo $_SESSION['username'];
				echo '<br> You are already logged in </h1>';
				$id = true;
				echo '<a class = "row justify-content-center" href="logout.php" tite ="logout"> Logout from session</a></div>';
				die();
			}
		?>

         <?php
		 	if ($id == false) {

				 $msg = '';

				 if (isset($_POST['login']) && !empty($_POST['username'])
					&& !empty($_POST['password'])) {

					if ($_POST['username'] == 'admin' &&
					   $_POST['password'] == 'password') {
					   $_SESSION['valid'] = true;
					   $_SESSION['username'] = 'admin';

					   echo 'You are logged in';
					   header("Refresh:0");
					}else {
					   $msg = 'Wrong username or password';
					}
				 }
			}
         ?>
      </div>

			<style>
		html, body {
			height: 100%;
		}

		.form-login {
			width: 100%;
			max-width: 330px;
			padding: 15px;
			margin: auto;
		}
		</style>
         <form class = "form-signin form-login" role = "form"
            action = "<?php echo htmlspecialchars($_SERVER['PHP_SELF']);
            ?>" method = "post">
			<h1 class="h3 mb-3 font-weight-normal">Webserv login</h1>
			<h4 class = "form-signin-heading"><?php if (isset($msg)) echo $msg; ?></h4>
            <input type = "text" class = "form-control mb-1"
               name = "username" placeholder = "admin"
               required autofocus></br>
            <input type = "password" class = "form-control mb-1"
               name = "password" placeholder = "password" required>
            <button class = "btn btn-lg btn-primary btn-block" type = "submit"
               name = "login">Login</button>
         </form>




   </body>
</html>
