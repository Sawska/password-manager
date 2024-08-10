chrome.runtime.onInstalled.addListener(() => {
  chrome.declarativeContent.onPageChanged.removeRules(undefined, () => {
      chrome.declarativeContent.onPageChanged.addRules([{
          conditions: [new chrome.declarativeContent.PageStateMatcher({
              pageUrl: { urlContains: '.' },
          })],
          actions: [new chrome.declarativeContent.ShowAction()]
      }]);
  });
});

chrome.webNavigation.onCompleted.addListener((details) => {
  chrome.scripting.executeScript({
      target: { tabId: details.tabId },
      files: ['content.js']
  });
}, { url: [{ urlMatches: 'https://*/*' }] });

chrome.runtime.onMessage.addListener((formObject) => {
  fetch('http://localhost:8080/store_form_data', {
      method: 'POST',
      headers: {
          'Content-Type': 'application/json'
      },
      body: JSON.stringify(formObject)
  })
  .then(response => response.json())
  .then(data => {
    console.log(data); 
  })
  .catch(error => {
    console.error('Error:', error);
  });
});
