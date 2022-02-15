<?php

    header('Content-type: image/png', true);
    $entityBody = file_get_contents('php://input');
    echo($entityBody);

    // phpinfo();
        // echo ("Content-type:text/html\r\n\r\n");
        // echo ("<html>\n");
        // echo ("<head>\n");
        // echo ("<title>Hello World - First CGI Program</title>\n");
        // echo ("</head>\n");
        // echo ("<body>\n");
        // echo ("<h2>Hello World! This is my first CGI program</h2>\n");
        // echo ("</body>\n");
        // echo ("</html>\n");
?>