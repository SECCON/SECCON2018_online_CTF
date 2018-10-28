let bip39 = require('bip39');
let crypto = require('crypto');
let wordlist_jp = bip39.wordlists["english"];
let question = new Buffer.from('Pz8/IHdlYWx0aCBicmVhZCBzcXVpcnJlbCBzb3J0IHVyYmFuIHBhZGRsZSBwYW5pYyBjb21wYW55IG1hdGVyaWFsIGJ1dHRlciB0aWx0IGNpdHkgaG9iYnkgc2V2ZW4gc2FtcGxlIGNhdXRpb24gaXZvcnkgY3VwIGJlY2F1c2UgcGllY2UgY3JpbWUgbWl4dHVyZSBhcnR3b3Jr', "base64").toString('ascii');

let questionArr = question.split(" ");
let arr = [];
for (let i=0; i < wordlist_jp.length; i++){
    questionArr[0] = wordlist_jp[i];
    let mnemonic = questionArr.join(' ');
    if (bip39.validateMnemonic(mnemonic, wordlist_jp)){
	let entropy = bip39.mnemonicToEntropy(mnemonic, wordlist_jp)
	if("583" == entropy.slice(0,3)){
	    md5 = crypto.createHash('md5');
	    console.log("SECCON{"+md5.update(entropy, 'binary').digest('hex')+"}");
	}
    }
}
