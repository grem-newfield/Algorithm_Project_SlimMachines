document.addEventListener("DOMContentLoaded", function() {
    function getCookie(name) {
      let cookieArray = document.cookie.split(';');
      for(let i = 0; i < cookieArray.length; i++) {
        let cookie = cookieArray[i];
        while (cookie.charAt(0) === ' ') {
          cookie = cookie.substring(1);
        }
        if (cookie.indexOf(name + "=") === 0) {
          return cookie.substring(name.length + 1, cookie.length);
        }
      }
      return "";
    }
  
    function sendIdWithRequest(userId) {
      fetch("/send-id", {
        method: "POST",
        headers: {
          'Content-Type': 'application/json',
        },
        body: JSON.stringify({ id: userId })
      })
      .then(response => response.json())
      .then(data => console.log('Success:', data))
      .catch((error) => {
        console.error('Error:', error);
      });
    }
  
    const userId = getCookie("userId");
    if (userId) {
      sendIdWithRequest(userId);
    }
  });
  