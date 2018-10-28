pragma solidity^0.4.24;

import "./Gacha.sol";

contract GachaFactory {
    address private owner;
    mapping(address => address) private userContract;//player address -> contract address
    
    modifier onlyOwner() {
        require(owner == msg.sender);
        _;
    }
    
    event Generate(address _player, address _contract);
    
    constructor () public {
        owner = msg.sender;
    }
    
    function generateContract(bytes32 _password, uint256 _seed, address _player) onlyOwner public returns(address) {
        Gacha gachaInstance = new Gacha(_password, _seed, _player);
        userContract[_player] = address(gachaInstance);
        emit Generate(_player, userContract[_player]);
        return userContract[_player];
    }
    
    function referContractAddress(address _player) onlyOwner public view returns(address) {
        return userContract[_player];
    }
    
    function validateContract(address _contract) onlyOwner public view returns(bool) {
        Gacha gachaInstance = Gacha(_contract);
        return gachaInstance.getItem();
    }
}