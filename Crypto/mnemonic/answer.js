let bip39 = require('bip39');
let crypto = require('crypto');
let wordlist_jp = bip39.wordlists["japanese"];
let question = '???　とかす　なおす　よけい　ちいさい　さんらん　けむり　ていど　かがく　とかす　そあく　きあい　ぶどう　こうどう　ねみみ　にあう　ねんぐ　ひねる　おまいり　いちじ　ぎゅうにく　みりょく　ろしゅつ　あつめる';

let questionArr = question.split("　");
let arr = [];
for (let i=0; i < wordlist_jp.length; i++){
    questionArr[0] = wordlist_jp[i];
    let mnemonic = questionArr.join(' ');
    if (bip39.validateMnemonic(mnemonic, wordlist_jp)){
	let entropy = bip39.mnemonicToEntropy(mnemonic, wordlist_jp)
	if("c0f" == entropy.slice(0,3)){
	    md5 = crypto.createHash('md5');
	    console.log("SECCON{"+md5.update(entropy, 'binary').digest('hex')+"}");
	}
    }
}
