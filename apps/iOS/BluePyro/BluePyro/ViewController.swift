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
    
    @IBOutlet weak var ExelitasLogo: UIImageView!
    @IBOutlet weak var ISYSTLogo: UIImageView!
    @IBOutlet weak var AppNameLabel: UILabel!
    
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
    @IBOutlet weak var mPickerDevice: UIPickerView!
    
    @IBOutlet weak var SelectDeviceLabel: UILabel!
    @IBOutlet weak var ThresholdLabel: UILabel!
    @IBOutlet weak var BlindTimeLabel: UILabel!
    @IBOutlet weak var PulseCounterLabel: UILabel!
    @IBOutlet weak var WindowsTimeLabel: UILabel!
    @IBOutlet weak var HPFLabel: UILabel!
    @IBOutlet weak var PulseModeLabel: UILabel!
    
    
    var pickerData: [String] = [String]()
    var DeviceList: [CBPeripheral] = []
    var bleCentral : CBCentralManager!
    var mBluePyro: CBPeripheral!
    let screenSize: CGRect = UIScreen.main.bounds
    let ratio = UIScreen.main.scale/UIScreen.main.nativeScale
    
    override func viewDidLoad() {
        super.viewDidLoad()
        // Do any additional setup after loading the view.
        bleCentral = CBCentralManager(delegate: self, queue: DispatchQueue.main)
        mPickerDevice.dataSource = self
        mPickerDevice.delegate = self
        
        ISYSTLogo.transform = CGAffineTransform(scaleX: ratio, y: ratio)
        ISYSTLogo.frame = CGRect(x: 0, y: 40, width: screenSize.width/2-10, height: ISYSTLogo.frame.height)
        ExelitasLogo.transform = CGAffineTransform(scaleX: ratio, y: ratio)
        ExelitasLogo.frame = CGRect(x: ISYSTLogo.frame.width, y: 40, width: screenSize.width/2, height: ExelitasLogo.frame.height)
        AdvCountLabel.frame = CGRect(x: screenSize.width/2 + 20, y: screenSize.height-40, width: AdvCountLabel.frame.width, height: AdvCountLabel.frame.height)
        countLabel.frame = CGRect(x: 10, y: screenSize.height-40, width: countLabel.frame.width, height: countLabel.frame.height)
        
        mDFUButton.transform = CGAffineTransform(scaleX: ratio, y: ratio)
        mDFUButton.frame = CGRect(x: 0, y: ISYSTLogo.frame.height + 50, width: screenSize.width/3 - 10, height: mDFUButton.frame.height)
        AppNameLabel.transform = CGAffineTransform(scaleX: ratio, y: ratio)
        AppNameLabel.frame = CGRect(x: mDFUButton.frame.width + 5, y: ISYSTLogo.frame.height + 50, width: screenSize.width/3 + 10, height: AppNameLabel.frame.height)
        mUpdateButton.transform = CGAffineTransform(scaleX: ratio, y: ratio)
        mUpdateButton.frame = CGRect(x: mDFUButton.frame.width + AppNameLabel.frame.width, y: ISYSTLogo.frame.height + 50, width: screenSize.width/3 , height: mUpdateButton.frame.height)
        mNameEdit.transform = CGAffineTransform(scaleX: ratio, y: ratio)
        mNameEdit.frame = CGRect(x: 10, y: ISYSTLogo.frame.height + mDFUButton.frame.height + 60, width: screenSize.width - 20, height: mNameEdit.frame.height)
        
        SelectDeviceLabel.transform = CGAffineTransform(scaleX: ratio, y: ratio)
        SelectDeviceLabel.frame = CGRect(x: 10, y: mNameEdit.frame.midY + 50, width: screenSize.width/2, height: SelectDeviceLabel.frame.height)
        mPickerDevice.transform = CGAffineTransform(scaleX: ratio, y: ratio)
        mPickerDevice.frame = CGRect(x: screenSize.width/2, y: mNameEdit.frame.midY + 25, width: screenSize.width/2 - 10, height: mPickerDevice.frame.height)
        
        ThresholdLabel.transform = CGAffineTransform(scaleX: ratio, y: ratio)
        ThresholdLabel.frame = CGRect(x: 10, y: SelectDeviceLabel.frame.midY + 50, width: screenSize.width/2, height: ThresholdLabel.frame.height)
        mThreshLabel.transform = CGAffineTransform(scaleX: ratio, y: ratio)
        mThreshLabel.frame = CGRect(x: screenSize.width/2, y: SelectDeviceLabel.frame.midY + 50, width: screenSize.width/2 - 10, height: mThreshLabel.frame.height)
        
        BlindTimeLabel.transform = CGAffineTransform(scaleX: ratio, y: ratio)
        BlindTimeLabel.frame = CGRect(x: 10, y: ThresholdLabel.frame.midY + 30, width: screenSize.width/2, height: BlindTimeLabel.frame.height)
        mBTimeLabel.transform = CGAffineTransform(scaleX: ratio, y: ratio)
        mBTimeLabel.frame = CGRect(x: screenSize.width/2, y: ThresholdLabel.frame.midY + 30, width: screenSize.width/2-10, height: mBTimeLabel.frame.height)
        
        PulseCounterLabel.transform = CGAffineTransform(scaleX: ratio, y: ratio)
        PulseCounterLabel.frame = CGRect(x: 10, y: BlindTimeLabel.frame.midY + 30, width: screenSize.width/2, height: PulseCounterLabel.frame.height)
        mPulseCntLabel.transform = CGAffineTransform(scaleX: ratio, y: ratio)
        mPulseCntLabel.frame = CGRect(x: screenSize.width/2, y: BlindTimeLabel.frame.midY + 30, width: screenSize.width/2-10, height: mPulseCntLabel.frame.height)
        
        WindowsTimeLabel.transform = CGAffineTransform(scaleX: ratio, y: ratio)
        WindowsTimeLabel.frame = CGRect(x: 10, y: PulseCounterLabel.frame.midY + 30, width: screenSize.width/2, height: WindowsTimeLabel.frame.height)
        mWinTimeLabel.transform = CGAffineTransform(scaleX: ratio, y: ratio)
        mWinTimeLabel.frame = CGRect(x: screenSize.width/2, y: PulseCounterLabel.frame.midY + 30, width: screenSize.width/2-10, height: mWinTimeLabel.frame.height)
        
        HPFLabel.transform = CGAffineTransform(scaleX: ratio, y: ratio)
        HPFLabel.frame = CGRect(x: 10, y: WindowsTimeLabel.frame.midY + 30, width: screenSize.width/2, height: HPFLabel.frame.height)
        mSwitchHpf.transform = CGAffineTransform(scaleX: ratio, y: ratio)
        mSwitchHpf.frame = CGRect(x: 3*screenSize.width/4 - mSwitchHpf.frame.width/2, y: WindowsTimeLabel.frame.midY + 30, width: screenSize.width/2-10, height: mSwitchHpf.frame.height)
        
        PulseModeLabel.transform = CGAffineTransform(scaleX: ratio, y: ratio)
        PulseModeLabel.frame = CGRect(x: 10, y: HPFLabel.frame.midY + 30, width: screenSize.width/2, height: PulseModeLabel.frame.height)
        mSwitchPulseMode.transform = CGAffineTransform(scaleX: ratio, y: ratio)
        mSwitchPulseMode.frame = CGRect(x: 3*screenSize.width/4 - mSwitchPulseMode.frame.width/2, y: HPFLabel.frame.midY + 30, width: screenSize.width/2-10, height: mSwitchPulseMode.frame.height)
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
                }
              
                var temp = UInt8(0)
                manData.getBytes(&temp, range: NSMakeRange(3, 1))
                
                var bytes: [UInt8] = [0,0,0,0,0,0,0,0]
                manData.getBytes(&bytes, range: NSMakeRange(8, manData.length-8))
                
                let name = String(bytes: bytes, encoding: .utf8)
                if name != nil && mModify == false {
                    mNameEdit.text = name
                    //let device_name = name! + peripheral.identifier.uuidString
                    //let device = Device(deviceName: name ?? " ", deviceAddress: peripheral.identifier.uuidString)
                    
                    if pickerData.count == 0{
                        pickerData.append(name ?? " ")
                        mPickerDevice.reloadAllComponents()
                        DeviceList.append(peripheral)
                    }else{
                        //let results = DeviceList.filter { $0.deviceAddress ==  device.deviceAddress}
                        //let exists = results.isEmpty == false
                        
                        /*if DeviceList.contains(where: { device in device.deviceAddress == peripheral.identifier.uuidString }) {
                            print("device exists in the list")
                        } else {
                            print("device does not exists in the list")
                        }*/
                        
                        if let device_in_list = DeviceList.enumerated().first(where: {$0.element.identifier == peripheral.identifier}) {
                           // do something with foo.offset and foo.element
                            print("device exists in the list: " + String(format: "%d", device_in_list.offset))
                            DeviceList.remove(at: device_in_list.offset)
                            pickerData.remove(at: device_in_list.offset)
                            
                            pickerData.append(name ?? " ")
                            mPickerDevice.reloadAllComponents()
                            DeviceList.append(peripheral)
                            
                            
                        } else {
                           // item could not be found
                            print("device does not exists in the list")
                            pickerData.append(name ?? " ")
                            mPickerDevice.reloadAllComponents()
                            DeviceList.append(peripheral)
                        }
                    }
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
                       //self.bleCentral.cancelPeripheralConnection(self.mBluePyro)
                       //mModify = false
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
                        let appData =  Data(bytes)
                        let messageData = NSMutableData() //or var messageData : NSMutableData = NSMutableData()
                        let str = mNameEdit.text ?? ""
                        print(str)
                        var nameData = str.data(using: String.Encoding.utf8)!
                        print(nameData.count)
                        //print(String(data: nameData, encoding: String.Encoding.utf8))
                        if nameData.count<8 {
                            //let zeros: [UInt8] = [0,0,0,0]
                            //let zerosArr = [UInt8](count: 8-nameData.count, repeatedValue: 0)
                            let zerosArr = Data(Array(repeating: 0, count: 8-nameData.count))
                            messageData.append(nameData)
                            messageData.append(zerosArr)
                            messageData.append(appData)
                        }else if nameData.count>8{
                            nameData.removeSubrange(8..<nameData.count)
                            messageData.append(nameData)
                            messageData.append(appData)
                        }else if nameData.count==8{
                            messageData.append(nameData)
                            messageData.append(appData)
                        }
                        
                        //print(messageData)
                        // Check if it has the write property
                        //if mCfgChar.properties.contains(.writeWithoutResponse) && self.mBluePyro != nil {
                        if self.mBluePyro != nil {
                            self.mBluePyro.writeValue(messageData as Data, for: mCfgChar, type: .withResponse)
                            
                        }
                        
                        //self.bleCentral.cancelPeripheralConnection(self.mBluePyro)
                        //mModify = false

                    }
                }
            }
            DispatchQueue.main.asyncAfter(deadline: .now() + 2, execute: {
                print("Update successful")
                self.bleCentral.cancelPeripheralConnection(self.mBluePyro)
                self.mModify = false
               })
            
             
            
        }
    }
    
    // Button click even handle
    @IBAction func mDFUButtonClick(_ sender: UIButton) {
        print("DFU button clicked")
        mButtonClicked = 1
        if self.mBluePyro != nil {
            self.bleCentral.connect(self.mBluePyro, options: nil)
        }
        //self.bleCentral.cancelPeripheralConnection(self.mBluePyro)
    }
    
    @IBAction func mUpdateButtonClick(_ sender: UIButton) {
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
    
    override func touchesBegan(_ touches: Set<UITouch>, with event: UIEvent?) {
        view.endEditing(true)
    }


}

extension ViewController: UIPickerViewDataSource{
    func numberOfComponents(in pickerView: UIPickerView) -> Int {
        return 1
    }
    
    func pickerView(_ pickerView: UIPickerView, numberOfRowsInComponent component: Int) -> Int {
        return pickerData.count
    }
}

extension ViewController: UIPickerViewDelegate{
    func pickerView(_ pickerView: UIPickerView, titleForRow row: Int, forComponent component: Int) -> String? {
        return pickerData[row]
    }
    func pickerView(_ pickerView: UIPickerView, didSelectRow row: Int, inComponent component: Int) {
        mNameEdit.text = pickerData[row]
        self.mBluePyro = DeviceList[row]
        print(self.mBluePyro.identifier.uuidString)
        print("\n")
        
        mModify = true
        mUpdateButton.setTitle("UPDATE", for: UIControl.State.normal)
    }
}
