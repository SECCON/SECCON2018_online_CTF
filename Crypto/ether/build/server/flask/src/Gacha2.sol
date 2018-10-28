pragma solidity^0.4.24;

contract Gacha2 {
    address public owner;
    address public player;
    uint256 public played = 0;
    uint256 public seed;
    uint256 public lastHash;
    bool public getItem = false;
    bytes32 private password;
    
    modifier onlyOwner() {
        require(owner == msg.sender);
        _;
    }
    
    constructor(bytes32 _password, uint256 _seed, address _player) public {
        owner = msg.sender;
        player = _player;
        password = _password;
        lastHash = uint256(blockhash(block.number-1));
        seed = _seed;
    }
    
    function pickUp() public returns(bool) {
        require(player == msg.sender);
        
        uint256 blockValue = uint256(blockhash(block.number-1));
        if (lastHash == blockValue) {
            revert();
        }
        lastHash = blockValue;

        played++;
        if (mod(played, 1000000) == 0) {
            getItem = true;    
            return getItem;
        }
        
        uint256 result = mod(seed * block.number, 200000);
        seed = result;
        
        if (result == 12345) getItem = true; // Flag is here!!
        
        return getItem;
    }
    
    function mod(uint256 a, uint256 b) internal pure returns (uint256) {
        require(b != 0);
        return a % b;
    }
    
    function initSeed(uint256 _seed) onlyOwner public {
        seed = _seed;
    }
    
    function changeOwner(bytes _password) public returns(bool){
        if (checkPassword(_password) == true) {
            owner = msg.sender;
        }
            
    }
    
    function checkPassword(bytes _password) public view returns(bool) {
        if (password == keccak256(abi.encodePacked(_password))) {
            return true;
        }
        else return false;
    }
    
}
