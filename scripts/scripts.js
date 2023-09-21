// scripts.js
// this contains all the scripts used for the website

// openMenu is for redirecting to different pages
// scripts.js
function openMenu(event, menuName) {
    var i, tabcontent, tablinks;
    
    // Get all elements with class "sensor" and hide them
    tabcontent = document.getElementsByClassName("sensor");
    for (i = 0; i < tabcontent.length; i++) {
      tabcontent[i].style.display = "none";
    }
    
    // Get all elements with class "tablink" and remove the "active" class
    tablinks = document.getElementsByClassName("tablink");
    for (i = 0; i < tablinks.length; i++) {
      tablinks[i].classList.remove("active");
    }
    
    // Show the current tab, and add an "active" class to the button that opened the tab
    document.getElementById(menuName).style.display = "block";
    event.currentTarget.classList.add("active");
  }
  
  // Add an event listener to initialize the first tab when the page loads
  window.addEventListener("load", function() {
    document.querySelector(".tablink").click();
  });


// function to open and close menu
function toggleMenu(event, menuName) {
  var tabcontent = document.getElementById(menuName);
  var computedStyle = window.getComputedStyle(tabcontent);

  if (computedStyle.display === "block") {
    tabcontent.style.display = "none";
    event.currentTarget.classList.remove("active");
  } else {
    tabcontent.style.display = "block";
    event.currentTarget.classList.add("active");
  }
}

window.addEventListener("load", function () {
  document.querySelector(".tablink").click();
});

// Function to load and insert HTML content into a specified container
function loadHTMLContent(url, containerId) {
  const container = document.getElementById(containerId);

  fetch(url)
      .then(response => response.text())
      .then(data => {
          container.innerHTML = data;
      })
      .catch(error => {
          console.error("Error loading HTML content:", error);
      });
}

 
// smoothScrollTo is used to integrate smooth scrolling into the website
function smoothScrollTo(targetId) {
  const targetElement = document.getElementById(targetId);

  if (targetElement) {
    window.scrollTo({
      top: targetElement.offsetTop,
      behavior: 'smooth'
    });
  }
};

// Function for Animation with GSAP
function animateWithGSAP(elementId) {
    const elementToAnimate = document.getElementById(elementId);
  
    gsap.from(elementToAnimate, {
      opacity: 0,
      y: 50, // Move 50 pixels down
      duration: 1, // Animation duration in seconds
      ease: 'power2.inOut' // Easing function
    });
  }

// navigation toggle
function toggleNavigation() {
  const menuIcon = document.getElementById('menu-icon');
  const navMenu = document.getElementById('nav-menu');

  menuIcon.addEventListener('click', () => {
    navMenu.classList.toggle('open');
  });
}

// function to validate input
function validateInput(value) {
  if (isNaN(value)) {
      return "Please enter a valid numeric value.";
  }
  return null;  
}

// function for conditions for each sensor
function updateStatus(condition, inputId, statusId) {
  var inputElement = document.getElementById(inputId);
  var statusElement = document.getElementById(statusId);

  // Clear previous status and error messages
  statusElement.innerHTML = "";
  inputElement.style.border = "1px solid #ccc";

  var value = parseFloat(inputElement.value);
  var validationError = validateInput(value);

  if (validationError !== null) {
      statusElement.innerHTML = validationError;
      inputElement.style.border = "1px solid red";
      return;
  }

  // Perform condition-specific logic based on the provided condition
  switch (condition) {
      case "temperature":
        if (value >= 30 && value <= 35) {
          result = "GOOD";
          statusElement.style.backgroundColor = "green";
          } else {
          result = "BAD";
          statusElement.style.backgroundColor = "red";
          }
          break;

      case "pH":
        if (value >= 0 && value <= 14) {
            if (value >= 5 && value <= 7) {
                result = "GOOD";
                statusElement.style.backgroundColor = "green";
            } else {
                result = "BAD";
                statusElement.style.backgroundColor = "red";
            }
        } else {
            result = "Out of acceptable pH range (0-14)";
            statusElement.style.backgroundColor = "red";
        }
        break;
      

          
      default:
          statusElement.innerHTML = "Condition not recognized.";
          break;
  }

  // Display the result
  statusElement.innerHTML = result;
}


// Function to open questions on FAQ page
function toggleAnswer(questionId) {
  var answer = document.getElementById(questionId);
  var allAnswers = document.querySelectorAll('.faq-container .faq-body');

  allAnswers.forEach(function (el) {
    if (el.id === questionId) {
      el.classList.toggle('open');
    } else {
      el.classList.remove('open');
    }
  });
}





