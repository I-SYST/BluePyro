//
//  ViewController.swift
//  BluePyro
//
//  Created by Tai on 2021-08-30.
//

import UIKit
import CoreBluetooth
class BluePyroPeripheral: NSObject {

        public static let BLUEPYRO_SERVICE_UUID   = CBUUID.init(string: "a008FFE0-01fb-4109-8a0a-1816898a6725")
        public static let BLUEPYRO_CFGCHAR_UUID   = CBUUID.init(string: "a008FFE1-01fb-4109-8a0a-1816898a6725")
        public static let BLUEPYRO_DFUCHAR_UUID   = CBUUID.init(string: "a008FFE2-01fb-4109-8a0a-1816898a6725")
    }

class ViewController: UIViewController,  CBCentralManagerDelegate, CBPeripheralDelegate {
    var mDetCnt = 0
    var mAdvCnt = 0
    var mButtonClicked = 0
    var mModify = false
    var mName: [UInt8] = [0,0,0,0,0,0,0,0]
    //var mDfuChar: CBCharacteristic
    //var mCfgChar: CBCharacteristic
    @IBOutlet weak var nameLabel: UILabel!
    
    @IBOutlet weak var uuidLable: UILabel!
    @IBOutlet weak var IdLabel: UILabel!
    @IBOutlet weak var AdvCountLabel: UILabel!
    @IBOutlet weak var countLabel: UILabel!
    
    @IBOutlet weak var mNameEdit: UITextField!
    
    @IBOutlet weak var mThreshLabel: UITextField!
    @IBOutlet weak var mBTimeLabel: UITextField!
    @IBOutlet weak var mPulseCntLabel: UITextField!
    @IBOutlet weak var mWinTimeLabel: UITextField!
    
    @IBOutlet weak var mUpdateButton: UIButton!
    @IBOutlet weak var mDFUButton: UIButton!
    @IBOutlet weak var mSwitchHpf: UISwitch!
    @IBOutlet weak var mSwitchPulseMode: UISwitch!
    
    
    var bleCentral : CBCentralManager!
    var mBluePyro: CBPeripheral!
    //@IBOutlet weak var IDLabel: NSTextField!
    
    override func viewDidLoad() {
        super.viewDidLoad()
        // Do any additional setup after loading the view.
        bleCentral = CBCentralManager(delegate: self, queue: DispatchQueue.main)
    }

    func centralManagerDidUpdateState(_ central: CBCentralManager) {
        switch central.state {
            
        case CBManagerState.poweredOff:
            print("CoreBluetooth BLE hardware is powered off")
            break
        case CBManagerState.poweredOn:
            print("CoreBluetooth BLE hardware is powered on and ready")
            
            bleCentral.scanForPeripherals(withServices: nil, options: nil)
            //bleCentral.scanForPeripherals(withServices: [BluePyroPeripheral.BLUEPYRO_SERVICE_UUID], options: [CBCentralManagerScanOptionAllowDuplicatesKey : true])
            break
        case CBManagerState.resetting:
            print("CoreBluetooth BLE hardware is resetting")
            break
        case CBManagerState.unauthorized:
            print("CoreBluetooth BLE state is unauthorized")
            
            break
        case CBManagerState.unknown:
            print("CoreBluetooth BLE state is unknown")
            break
        case CBManagerState.unsupported:
            print("CoreBluetooth BLE hardware is unsupported on this platform")
            break
            
        @unknown default:
            print("CoreBluetooth BLE state is unknown")
            break
        }
    }
    
    func centralManager(_ central: CBCentralManager, didDiscover peripheral: CBPeripheral, advertisementData: [String : Any], rssi RSSI: NSNumber) {
        if let pname = peripheral.name {
            if pname == "BluePyro" {
                //self.bleCentral.stopScan()
                
                self.mBluePyro = peripheral
                self.mBluePyro.delegate = self
                
                
                nameLabel.text = self.mBluePyro.name
                uuidLable.text = self.mBluePyro.identifier.uuidString
                //print(self.mBluePyro.identifier)
                //self.bleCentral.connect(self.mBluePyro, options: nil)
                if advertisementData[CBAdvertisementDataManufacturerDataKey] == nil {
                    return
                }
                
                var manId = UInt16(0)
                let manData = advertisementData[CBAdvertisementDataManufacturerDataKey] as! NSData
                
                if manData.length < 3 {
                    return
                }
              
                manData.getBytes(&manId, range: NSMakeRange(0, 2))
                if manId != 0x177 {
                    return
                }else {
                    IdLabel.text = "ManuID: " + String(format: "%d", manId)
                }
              
                var temp = UInt8(0)
                manData.getBytes(&temp, range: NSMakeRange(3, 1))
                
                var bytes: [UInt8] = [0,0,0,0,0,0]
                manData.getBytes(&bytes, range: NSMakeRange(8, manData.length-8))
                
                let name = String(bytes: bytes, encoding: .utf8)
                if name != nil && mModify == false {
                    mNameEdit.text = name
                    
                    if temp == 0 {
                        mNameEdit.backgroundColor = UIColor.white
                        mWinTimeLabel.text = String(format: "%d", 0)
                        mPulseCntLabel.text = String(format: "%d", 0)
                        mBTimeLabel.text = String(format: "%d", 0)
                        mThreshLabel.text = String(format: "%d", 0)
                        mSwitchPulseMode.setOn(false, animated: true)
                        mSwitchHpf.setOn(false, animated: true)
                    }else{
                        mDetCnt += 1
                        print("Detected count: " + String(format: "%d", mDetCnt))
                        countLabel.text = "Detected count: " + String(format: "%d", mDetCnt)
                        mNameEdit.backgroundColor = UIColor.red
                    }
                    
                }
                
                if mModify == false {
                    
                    var p = 0
                    manData.getBytes(&p, range: NSMakeRange(4, 1))
                    p = p & 0x1
                    if p != 0 {
                        mSwitchPulseMode.setOn(true, animated: true)
                    }
                    else{
                        mSwitchPulseMode.setOn(false, animated: true)
                    }
                    
                    var hpf = 0
                    manData.getBytes(&hpf, range: NSMakeRange(4, 1))
                    hpf = hpf & 0x4
                    if hpf != 0 {
                        mSwitchHpf.setOn(true, animated: true)
                    }
                    else{
                        mSwitchHpf.setOn(false, animated: true)
                    }
                    
                    var wt = 0
                    var pc = 0
                    var bt1 = 0, bt2 = 0
                    var thr1 = 0, thr2 = 0
                    manData.getBytes(&wt, range: NSMakeRange(5, 1))
                    manData.getBytes(&pc, range: NSMakeRange(5, 1))
                    manData.getBytes(&bt1, range: NSMakeRange(5, 1))
                    manData.getBytes(&thr1, range: NSMakeRange(6, 1))
                    manData.getBytes(&bt2, range: NSMakeRange(6, 1))
                    manData.getBytes(&thr2, range: NSMakeRange(7, 1))
                    wt = (wt >> 1) & 0x3
                    pc = (pc >> 3) & 0x3
                    bt1 = (bt1 >> 5) & 0x7
                    thr1 = (thr1 >> 1) & 0x7f
                    bt2 = (bt2 & 0x1) << 0x3
                    thr2 = (thr2 & 0x1) << 0x7
                    
                    mWinTimeLabel.text = String(format: "%d", wt)
                    mPulseCntLabel.text = String(format: "%d", pc)
                    mBTimeLabel.text = String(format: "%d", bt1|bt2)
                    mThreshLabel.text = String(format: "%d", thr1|thr2)
                  
                    
                }
                
                mAdvCnt += 1
                print("Advertise count: " + String(format: "%d", mAdvCnt))
                AdvCountLabel.text = "Advertise count: " + String(format: "%d", mAdvCnt)
            }
        }
    }
    func centralManager(_ central: CBCentralManager, didConnect peripheral: CBPeripheral) {
        if self.mBluePyro == peripheral{
            //peripheral.delegate = self
            self.mBluePyro.discoverServices([BluePyroPeripheral.BLUEPYRO_SERVICE_UUID])
            print("Connected to BluePyro peripheral")
        }
    }
    func centralManager(_ central: CBCentralManager,
                        didDisconnectPeripheral peripheral: CBPeripheral,
                        error: Error?) {
        print("Disconnected from BluePyro peripheral")
        
        
    }
    
    func centralManager(_ central: CBCentralManager, didFailToConnect peripheral: CBPeripheral, error: Error?) {
    }
    
    func scanPeripheral(_ sender: CBCentralManager)
    {
        print("Scan for peripherals")
        bleCentral.scanForPeripherals(withServices: nil, options: nil)
        //bleCentral.scanForPeripherals(withServices: [BluePyroPeripheral.BLUEPYRO_SERVICE_UUID], options: [CBCentralManagerScanOptionAllowDuplicatesKey : true])
    }

    // Handles discovery event
    func peripheral(_ peripheral: CBPeripheral, didDiscoverServices error: Error?) {
        if let services = peripheral.services {
            for service in services {
                if service.uuid == BluePyroPeripheral.BLUEPYRO_SERVICE_UUID {
                    print("BluePyro service found")
                    //Now kick off discovery of characteristics
                    peripheral.discoverCharacteristics([BluePyroPeripheral.BLUEPYRO_DFUCHAR_UUID,BluePyroPeripheral.BLUEPYRO_CFGCHAR_UUID], for: service)
                    return
                }
            }
        }
    }
    // Handling discovery of characteristics
    func peripheral(_ peripheral: CBPeripheral, didDiscoverCharacteristicsFor service: CBService, error: Error?) {
        var mDfuChar: CBCharacteristic
        var mCfgChar: CBCharacteristic
        if let characteristics = service.characteristics {
            for characteristic in characteristics {
                if characteristic.uuid == BluePyroPeripheral.BLUEPYRO_DFUCHAR_UUID {
                    print("DFU characteristic found")
                    mDfuChar = characteristic
                    if mButtonClicked == 1 {
                       
                       let dfuval = 0xff
                       let bytes = withUnsafeBytes(of: dfuval.bigEndian, Array.init)
                       print(bytes)
                       let appData = Data(bytes)
                       // Check if it has the write property
                       if mDfuChar.properties.contains(.writeWithoutResponse) && self.mBluePyro != nil {
                           
                           self.mBluePyro.writeValue(appData, for: mDfuChar, type: .withoutResponse)
                       }
                       self.bleCentral.cancelPeripheralConnection(self.mBluePyro)
                       mModify = false
                   }
                    
                } else if characteristic.uuid == BluePyroPeripheral.BLUEPYRO_CFGCHAR_UUID {
                    print("CFG characteristic found");
                    mCfgChar = characteristic
                    if mButtonClicked == 0
                    {
            
                        let thresh  = (Int(mThreshLabel.text!) ?? 0) & 0xff
                        let btime = (Int(mBTimeLabel.text!) ?? 0) & 0xf
                        let wtime = (Int(mWinTimeLabel.text!) ?? 0) & 0x3
                        let pcnt = (Int(mPulseCntLabel.text!) ?? 0) & 0x3
                        
                        //let hpf =  mSwitchHpf.isOn? 1 : 0
                        //let pmode = mSwitchPulseMode.isOn? 1 : 0
                        
                        var hpf = 0
                        if mSwitchHpf.isOn
                        {
                            hpf = 1
                        }else{
                            hpf = 0
                        }
                        var pmode = 0
                        if mSwitchPulseMode.isOn
                        {
                            pmode = 1
                        }
                        else{
                            pmode = 0
                        }
                       
                        let cfgval = (thresh << 17)|(btime << 13)|(pcnt << 11)|(wtime << 9)|(hpf << 2)|(pmode)|(0x10)
                        let cfgval2 = Int32(cfgval)
                        let bytes = withUnsafeBytes(of: cfgval2.bigEndian, Array.init)
                        print(bytes)
                        let str = mNameEdit.text ?? ""
                       
                        let nameData = str.data(using: String.Encoding.utf8)!
                        print(String(data: nameData, encoding: String.Encoding.utf8))
                        let appData =  Data(bytes)
                        let zeros: [UInt8] = [0,0,0,0]
                        let messageData = NSMutableData() //or var messageData : NSMutableData = NSMutableData()
                        messageData.append(nameData)
                        messageData.append(Data(zeros))
                        messageData.append(appData)
                        
                        //print(messageData)
                        // Check if it has the write property
                        if mCfgChar.properties.contains(.writeWithoutResponse) && self.mBluePyro != nil {
                        if self.mBluePyro != nil {
                            
                            self.mBluePyro.writeValue(messageData as Data, for: mCfgChar, type: .withoutResponse)
                        }
                        
                        self.bleCentral.cancelPeripheralConnection(self.mBluePyro)
                        mModify = false

                    }
                }
            }
            
             
            
        }
    }
    
    // Button click even handle
    @IBAction func mDFUButtonClick(_ sender: Any) {
        print("DFU button clicked")
        mButtonClicked = 1
        if self.mBluePyro != nil {
            self.bleCentral.connect(self.mBluePyro, options: nil)
        }
        //self.bleCentral.cancelPeripheralConnection(self.mBluePyro)
    }
    
    @IBAction func mUpdateButtonClick(_ sender: Any) {
        print("Update button clicked")
        
        if mModify == true{
            mButtonClicked = 0
            // Connect!
            if self.mBluePyro != nil {
                self.bleCentral.connect(self.mBluePyro, options: nil)
            }
            mUpdateButton.setTitle("MODIFY", for: UIControl.State.normal)
        }
        else {
            mModify = true
            mUpdateButton.setTitle("UPDATE", for: UIControl.State.normal)
        }
    }
    
    
    
    
    

}

}
