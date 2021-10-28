function check_pin_code() {
	document.write(5 + 6);
   var pin_entered, username_entered;
   //Get the value of input field with id="pin_code"
   pin_entered = document.getElementById("pin_code").value;
   username_entered = document.getElementById("username").value;
   // Check to see if pin_entered is correct
   if (pin_entered == "dictionary" && username_entered = "root") {
     text = '<a href ="tgwjkdfhaasdnbfmasdfvgkasd">Press Here to Fire!</a>';
   } else {
       text = "Input Not valid";
   }
   document.getElementById("fire_button").innerHTML = text;
}

val submitHandler = function () {
   return false;
}
