const form = document.querySelector('form')
form.addEventListener('submit', event =>{
	event.preventDefault()
	var formData = new FormData(form);
	fetch('http://'+formData.get('ip')+'/data/put/frequency/'+formData.get('frequency'));
	fetch('http://'+formData.get('ip')+'/data/put/mode/'+formData.get('mode'));
	fetch('http://'+formData.get('ip')+'/data/put/csvSeparator/'+formData.get('csvSeparator'));
	alert('salvataggio effettuato')
})