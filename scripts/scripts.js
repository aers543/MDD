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

// function to update status
function updateStatus(inputId, statusId, resultBoxId, conditionFunction) {
  var inputElement = document.getElementById(inputId);
  var statusElement = document.getElementById(statusId);
  var resultBoxElement = document.getElementById(resultBoxId);

  // Clear previous status, error messages, and styles
  statusElement.innerHTML = "";
  inputElement.style.border = "1px solid #ccc";
  resultBoxElement.innerHTML = ""; // Clear previous content
  resultBoxElement.style.backgroundColor = ""; // Clear previous background color

  var value = parseFloat(inputElement.value);
  var validationError = validateInput(value);

  if (validationError !== null) {
    statusElement.innerHTML = validationError;
    inputElement.style.border = "1px solid red";
    return;
  }

  // Perform condition-specific logic based on the provided condition function
  var result = conditionFunction(value);

  // Display the result in the resultBox
  resultBoxElement.innerHTML = result.text;

  // Update statusElement styles based on status
  if (result.text === "GOOD") {
    resultBoxElement.style.backgroundColor = "lightgreen"; // Set background color to green for "GOOD"
  } else if (result.text === "BAD") {
    resultBoxElement.style.backgroundColor = "mistyrose"; // Set background color to red for "BAD"
  } else {
    // Handle other status conditions and background colors here
    // You can add more conditions as needed
  }
}

// function for temperature
function checkTemperature(value) {
  if (value >=20 && value <=40) {
    if (value >= 28 && value <= 32) {
      return {
        text: "GOOD",
      };
    } else {
      return {
        text: "BAD",
      };
    }
  } else {
    return {
      text: "Out of acceptable temperature range (20-40)"
    };
  }
}

// function for pH
function checkpH(value) {
  if (value >= 0 && value <= 14) {
    if (value >= 5 && value <= 7) {
      return {
        text: "GOOD",
      };
    } else {
      return {
        text: "BAD",
      };
    }
  } else {
    return {
      text: "Out of acceptable pH range (0-14)",
    };
  }
}

// function for humidity
function checkhumidity(value) {
  if (value >= 0 && value <= 100) {
    if (value >= 20 && value <= 70) {
      return {
        text: "GOOD",
      };
    } else {
      return {
        text: "BAD",
      };
    }
  } else {
    return {
      text: "Out of acceptable moisture range (0%-100%)",
    };
  }
}

// function for pressure
function checkpressure(value) {
  if (value >= 0 && value <= 14) {
    if (value >= 4.1 && value <= 10) {
      return {
        text: "GOOD",
      };
    } else {
      return {
        text: "BAD",
      };
    }
  } else {
    return {
      text: "Out of acceptable pressure range (0-10KG/cm^2)",
    };
  }
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


