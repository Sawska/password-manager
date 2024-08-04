chrome.runtime.onIstalled.addListener(() => {
    chrome.declarativeContent.onPageChange.removeRules(undefined, () => {
        chrome.declarativeContent.onPageChange.addRules([{
            conditions: [new chrome.declarativeContent.PageStateMatcher({
            pageUrl: {urlContains: '.'},
            })],
            actions: [new chrome.declarativeContent.ShowPageAction()]
        }]);
    });
});

chrome.webNavigation.onCompleted.addListener(() => {
    chrome.tabs.executeScript({
      file: 'content.js'
    });
  }, {url: [{urlMatches : 'https://*/*'}]});


chrome.runtime.onMessage.addListener((formObject) => {
    fetch('http://localhost:8080/store_form_data', {
      method: 'POST',
      headers: {
        'Content-Type': 'application/json'
      },
      body: JSON.stringify(formObject)
    }).then(response => response.json())
      .then(data => console.log(data));
  });