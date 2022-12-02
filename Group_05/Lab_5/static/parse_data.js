if (!!window.EventSource) {
    var source = new EventSource('/');
    source.onmessage = function(e) {
      var bumper = e.data[1]
      var cliff = e.data[3];
      var drop = e.data[5];


      // finish the code to handle the bumper status
        if (bumper=="0")
          {
            document.getElementById("bumper_stat").value = "OFF";
          }
        if (bumper=="1")
        {
          document.getElementById("bumper_stat").value = "Right";
        }
        if (bumper=="2")
        {
          document.getElementById("bumper_stat").value = "Center";
        }
        if (bumper=="4")
        {
          document.getElementById("bumper_stat").value = "Left";
        }

         
        
        // finish the code to handle the wheel drop status 
      if (drop=="0")
        {
          document.getElementById("wheel_stat").value = "OFF";

        }
        if (drop=="1")
        {
          document.getElementById("wheel_stat").value = "Right";

        }
        if (drop=="2")
        {
          document.getElementById("wheel_stat").value = "Left";

        }

      // finish the code to handle cliff status 
      if (cliff=="0")
        {
          document.getElementById("cliff_stat").value = "OFF";
        }
        if (cliff=="1")
        {
          document.getElementById("cliff_stat").value = "Right";
        }
        if (cliff=="2")
        {
          document.getElementById("cliff_stat").value = "Center";
        }
        if (cliff=="4")
        {
          document.getElementById("cliff_stat").value = "Left";
        }

    }
  }