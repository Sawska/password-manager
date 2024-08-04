document.addEventListener('submit', (event) => {
    const form = event.target;
    const formData = new FormData(form);
    const formObject = {};
    formData.forEach((value, key) => {
      formObject[key] = value;
    });
  
    chrome.runtime.sendMessage(formObject);
  });