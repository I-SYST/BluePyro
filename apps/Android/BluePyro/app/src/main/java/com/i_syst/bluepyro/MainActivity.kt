package com.i_syst.bluepyro

import android.Manifest
import android.bluetooth.*
import android.bluetooth.le.BluetoothLeScanner
import android.bluetooth.le.ScanCallback
import android.bluetooth.le.ScanResult
import android.content.Context
import android.os.Bundle
import android.util.Log
import android.view.View
import android.widget.Button
import android.widget.Spinner
import android.widget.Switch
import android.widget.TextView
import androidx.appcompat.app.AppCompatActivity
import java.nio.ByteBuffer
import java.nio.ByteOrder
import java.nio.charset.Charset
import java.util.*
import android.widget.ArrayAdapter

import java.util.ArrayList
import android.R.attr.name
import android.widget.AdapterView

import android.widget.AdapterView.OnItemSelectedListener

import android.R.attr.name
import android.bluetooth.BluetoothDevice

import android.widget.Toast

import android.R.attr.name
import android.text.method.LinkMovementMethod


internal class Device(var deviceName: String, var deviceAddress: String) {

    override fun toString(): String {
        return "$deviceName"
    }

    override fun equals(obj: Any?): Boolean {
        if (this === obj) return true
        if (obj !is Device) return false
        return deviceAddress == obj.deviceAddress
    }
}
class MainActivity : AppCompatActivity() {

    val BLUEPYRO_SERVICE_UUID =
        UUID.fromString("a008FFE0-01fb-4109-8a0a-1816898a6725")
    val BLUEPYRO_CFGCHAR_UUID =
        UUID.fromString("a008FFE1-01fb-4109-8a0a-1816898a6725")
    val BLUEPYRO_DFUCHAR_UUID =
        UUID.fromString("a008FFE2-01fb-4109-8a0a-1816898a6725")
    val BLEADV_MANDATA_TYPE_ADC: Byte = 7
    val BLEADV_MANDATA_SN = 0xFF.toByte()
    val AVERAGE_COUNT = 20
    val BLEADV_MANDATA_TYPE_APP: Byte = 0

    var mBluetoothDevice: BluetoothDevice? = null
    var mBluetoothAdapter: BluetoothAdapter? = null
    var mBleGatt: BluetoothGatt? = null
    var mCfgChar: BluetoothGattCharacteristic? = null
    var mDfuChar: BluetoothGattCharacteristic? = null
    private val mCharWritCompleted = true
    var mCtx: Context? = null

    var mLEScanner: BluetoothLeScanner? = null
    private var mBTimeLabel: TextView? = null
    private var mTitleLabel : TextView? = null
    private var mRawLabel : TextView? = null
    private var mAdvCountLabel: TextView? = null
    private var mThreshLabel : TextView? = null
    private var mIdLabel : TextView? = null
    private var mPulseCntLabel : TextView? = null
    private var mHpfLabel : TextView? = null
    private var mWinTimeLabel : TextView? = null
    private var mUpdateButton : Button? = null
    private var mDFUButton : Button? = null
    private var mDeviceSpiner : Spinner? = null
    private var mDeviceList: MutableList<Device>? = null
    private var dataAdapter: ArrayAdapter<Device>? = null
    private var mPrivacyPolicyLabel : TextView? = null
    var zeroWeight = 0
    private var mZeroBut : Button? = null
    var mAvrStart : Boolean = false
    var mZeroVal : Float = 0.0F
    var mAvrCnt = 0
    private var mCalBut : Button? = null
    var mCalState = 0
    var mAvrVal = 0F
    var mScaleFactor = 20F
    var mWeight = 0F
    var mDetCnt = 0
    var mAdvCnt = 0
    //var mName = ByteArray(8)
    var mModify : Boolean = false
    var mButtonClicked = 0
    var mDeviceSelected : Boolean = false

    private val mGattCallback = object : BluetoothGattCallback() {
        override fun onConnectionStateChange(gatt: BluetoothGatt, status: Int, newState: Int) {//Change in connection state
            if (newState == BluetoothProfile.STATE_CONNECTED) {//See if we are connected
                //Log.i(TAG, "**ACTION_SERVICE_CONNECTED**$status")
                //broadcastUpdate(BLEConstants.ACTION_GATT_CONNECTED)//Go broadcast an intent to say we are connected
                gatt.discoverServices()
                //mBluetoothGatt?.discoverServices()//Discover services on connected BLE device
            } else if (newState == BluetoothProfile.STATE_DISCONNECTED) {//See if we are not connectedLog.i(TAG, "**ACTION_SERVICE_DISCONNECTED**" + status);
                //broadcastUpdate(BLEConstants.ACTION_GATT_DISCONNECTED)//Go broadcast an intent to say we are disconnected
            }
        }
        override fun onServicesDiscovered(gatt: BluetoothGatt, status: Int) {              //BLE service discovery complete
            if (status == BluetoothGatt.GATT_SUCCESS) {                                 //See if the service discovery was successful
                //Log.i(TAG, "**ACTION_SERVICE_DISCOVERED**$status")
                //Get the characteristic from the discovered gatt server
                if (mButtonClicked == 0) {
                    val service = gatt.getService(BLUEPYRO_SERVICE_UUID)
                    mCfgChar = service.getCharacteristic(BLUEPYRO_CFGCHAR_UUID)
                    mCfgChar?.writeType = BluetoothGattCharacteristic.WRITE_TYPE_DEFAULT

                    val tresh = Integer.valueOf(mThreshLabel?.text.toString()) and 0xff
                    val btime = Integer.valueOf(mBTimeLabel?.text.toString()) and 0xf
                    val wtime = Integer.valueOf(mWinTimeLabel?.text.toString()) and 3
                    val hpf =
                        if ((findViewById<View>(R.id.switchHpf) as Switch).isChecked()) 1 else 0 //Integer.valueOf(mHpfLabel?.text.toString()) and 1
                    val pcnt = Integer.valueOf(mPulseCntLabel?.text.toString()) and 3
                    val pmode =
                        if ((findViewById<View>(R.id.switchPulseMode) as Switch).isChecked()) 1 else 0
                    val cfgval =
                        (tresh shl 17) or (btime shl 13) or (pcnt shl 11) or (wtime shl 9) or (hpf shl 2) or (pmode) or (0x10)

                    val bytes = ByteArray(4)
                    ByteBuffer.wrap(bytes).order(ByteOrder.LITTLE_ENDIAN).putInt(cfgval)
                    var mName = ByteArray(8)

                    mName = mIdLabel?.text.toString().toByteArray()
                    if (mName.size<8) {
                        val zerosbytes = ByteArray(8-mName.size)
                        mName += zerosbytes
                    }else if(mName.size>8){
                        mName = Arrays.copyOfRange(mName,  0,  8)
                    }
                    val appData = mName + bytes

                    mCfgChar?.setValue(appData)
                    val res = mBleGatt!!.writeCharacteristic(mCfgChar)
                }
                else if (mButtonClicked == 1) {
                    val service =
                        gatt.getService(BLUEPYRO_SERVICE_UUID)
                    mDfuChar = service.getCharacteristic(BLUEPYRO_DFUCHAR_UUID)
                    mDfuChar?.writeType = BluetoothGattCharacteristic.WRITE_TYPE_DEFAULT
                    val bytes = ByteArray(1)
                    bytes[0] = 0xff.toByte()
                    val appData : ByteArray = bytes
                    mDfuChar?.setValue(appData)
                    val res = mBleGatt!!.writeCharacteristic(mDfuChar)
                }
                Log.i(" ssss" , " ddd")
                //broadcastUpdate(BLEConstants.ACTION_GATT_SERVICES_DISCOVERED)                       //Go broadcast an intent to say we have discovered services
            } else {                                                                      //Service discovery failed so log a warning
                //Log.i(TAG, "onServicesDiscovered received: $status")
            }
        }
        //For information only. This application uses Indication to receive updated characteristic data, not Read
        override fun onCharacteristicRead(gatt: BluetoothGatt, characteristic: BluetoothGattCharacteristic, status: Int) { //A request to Read has completed
//String value = characteristic.getStringValue(0);
//int value = characteristic.getIntValue(BluetoothGattCharacteristic.FORMAT_UINT32, 0);
            val values = characteristic.value
            val clearValue = byteArrayOf(255.toByte())
            var value = 0
            if (null != values) {
                //Log.i(TAG, "ACTION_DATA_READ VALUE: " + values.size)
                //Log.i(TAG, "ACTION_DATA_READ VALUE: " + (values[0] and 0xFF.toByte()))
                //value = (values[0] and 0xFF.toByte()).toInt()
            }
            //BLEConnectionManager.writeEmergencyGatt(clearValue)
            if (status == BluetoothGatt.GATT_SUCCESS) {
//See if the read was successful
                //Log.i(TAG, "**ACTION_DATA_READ**$characteristic")
                //broadcastUpdate(BLEConstants.ACTION_DATA_AVAILABLE, characteristic)                 //Go broadcast an intent with the characteristic data
            } else {
                //Log.i(TAG, "ACTION_DATA_READ: Error$status")
            }
        }
        //For information only. This application sends small packets infrequently and does not need to know what the previous write completed
        override fun onCharacteristicWrite(gatt: BluetoothGatt, characteristic: BluetoothGattCharacteristic, status: Int) { //A request to Write has completed
            if (status == BluetoothGatt.GATT_SUCCESS) {                                 //See if the write was successful
                //Log.e(TAG, "**ACTION_DATA_WRITTEN**$characteristic")
                //broadcastUpdate(BLEConstants.ACTION_DATA_WRITTEN, characteristic)                   //Go broadcast an intent to say we have have written data
                mBleGatt?.disconnect()
                mModify = false
            }
        }
        override fun onCharacteristicChanged(gatt: BluetoothGatt, characteristic: BluetoothGattCharacteristic?) {
            if (characteristic != null && characteristic.properties == BluetoothGattCharacteristic.PROPERTY_NOTIFY) {
                //Log.e(TAG, "**THIS IS A NOTIFY MESSAGE")
            }
            if (characteristic != null) {
                //broadcastUpdate(BLEConstants.ACTION_DATA_AVAILABLE, characteristic)
            }                     //Go broadcast an intent with the characteristic data
        }
    }
        //private var mAdapter: com.i_syst.blystscale.MainActivity.DeviceListAdapter? = null
/*
    private val mGattCallback: BluetoothGattCallback = object : BluetoothGattCallback() {
        override fun onConnectionStateChange(
            gatt: BluetoothGatt, status: Int,
            newState: Int
        ) {
            var intentAction: String
            if (newState == BluetoothProfile.STATE_CONNECTED) {
                gatt.discoverServices()
            } else if (newState == BluetoothProfile.STATE_DISCONNECTED) {
            }
        }

        // New services discovered
        override fun onServicesDiscovered(gatt: BluetoothGatt, status: Int) {
            if (status == BluetoothGatt.GATT_SUCCESS) {
                val services = gatt.services
                val service =
                    gatt.getService(BLUEPYRO_SERVICE_UUID) ?: return
                mCfgChar = service.getCharacteristic(BLUEPYRO_CFGCHAR_UUID)
                mCfgChar?.writeType = BluetoothGattCharacteristic.WRITE_TYPE_DEFAULT


               // val data_characteristic =
               //     service.getCharacteristic(MainActivity.BLUEIO_RDCHAR_UUID)
               // val descriptors =
               //     data_characteristic.descriptors
               // val descriptor = descriptors[0]
               // descriptor.value = BluetoothGattDescriptor.ENABLE_NOTIFICATION_VALUE
               // gatt.writeDescriptor(descriptor)
               // gatt.setCharacteristicNotification(data_characteristic, true)
            } else {
            }
        }

        // Result of a characteristic read operation
        override fun onCharacteristicRead(
            gatt: BluetoothGatt,
            characteristic: BluetoothGattCharacteristic,
            status: Int
        ) {
            if (status == BluetoothGatt.GATT_SUCCESS) {
            }
        }
/*
        override fun onCharacteristicChanged(
            gatt: BluetoothGatt,
            characteristic: BluetoothGattCharacteristic
        ) {
            val data = characteristic.value
            Log.i("onCharacteristicChanged", "called")
            when (data[0]) {
                MainActivity.BLEADV_MANDATA_TYPE_TPH -> {
                    val press = ByteBuffer.wrap(
                        data,
                        1,
                        4
                    ).order(ByteOrder.LITTLE_ENDIAN).int.toDouble() / 1000.0
                    val temp = ByteBuffer.wrap(
                        data,
                        5,
                        2
                    ).order(ByteOrder.LITTLE_ENDIAN).short.toDouble() / 100.0
                    val humi = ByteBuffer.wrap(
                        data,
                        7,
                        2
                    ).order(ByteOrder.LITTLE_ENDIAN).short.toDouble() / 100.0
                    var s = String.format("%.2f", press)
                    mPressLabel.setText(s)
                    mPressLabel.getRootView().postInvalidate()
                    s = String.format("%.2f", temp)
                    mTempLabel.setText(s)
                    mTempLabel.getRootView().postInvalidate()
                    s = String.format("%.0f", humi)
                    mHumiLabel.setText(s)
                    mHumiLabel.getRootView().postInvalidate()
                }
                MainActivity.BLEADV_MANDATA_TYPE_GAS -> {
                    val GasRes =
                        ByteBuffer.wrap(data, 1, 4).order(ByteOrder.LITTLE_ENDIAN)
                            .int
                    val AirQ =
                        ByteBuffer.wrap(data, 5, 2).order(ByteOrder.LITTLE_ENDIAN)
                            .short
                    s = String.format("%d", AirQ)
                    mAirQIdxLabel.setText(s)
                }
                MainActivity.BLEADV_MANDATA_TYPE_ADC -> {
                    val v = ByteBuffer.wrap(
                        data,
                        5,
                        4
                    ).order(ByteOrder.LITTLE_ENDIAN).float
                    s = String.format("%.2f", v)
                    mAdcLabel.setText(s)
                }
                MainActivity.BLEADV_MANDATA_SN -> {
                }
            }
        }
        */

    }
*/

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)
        requestPermissions(
            arrayOf(
                Manifest.permission.ACCESS_COARSE_LOCATION,
                Manifest.permission.ACCESS_FINE_LOCATION
            ), 1
        )
        mCtx = getBaseContext()
        val bluetoothManager =
            getSystemService(Context.BLUETOOTH_SERVICE) as BluetoothManager


        mUpdateButton = findViewById<View>(R.id.button) as Button
        mUpdateButton?.setOnClickListener {
                // Code here executes on main thread after user presses button
            //Log.d("onButtonClick")

            if (mModify == true) {
                mButtonClicked = 0
                if (mBluetoothDevice != null) {
                    mBleGatt = mBluetoothDevice?.connectGatt(mCtx, false, mGattCallback)
                }
                mUpdateButton?.text = "MODIFY"
            }
            else {
                mModify = true;
                mUpdateButton?.text = "UPDATE"
            }
        }

        mDFUButton = findViewById<View>(R.id.dfu) as Button
        mDFUButton?.setOnClickListener {
            mButtonClicked = 1
            if (mBluetoothDevice != null) {
                mBleGatt = mBluetoothDevice?.connectGatt(mCtx, false, mGattCallback)
            }
        }

        mDeviceList = ArrayList()
        mDeviceSpiner = findViewById<View>(R.id.spinnerDevice) as Spinner
        dataAdapter = ArrayAdapter(this, R.layout.spinner_item, mDeviceList as ArrayList<Device>)
        dataAdapter!!.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item)
        mDeviceSpiner!!.adapter = dataAdapter

        
        mDeviceSpiner!!.onItemSelectedListener = object : OnItemSelectedListener {
            override fun onItemSelected(
                parentView: AdapterView<*>?,
                selectedItemView: View,
                position: Int,
                id: Long
            ) {
                if(mDeviceSelected) {

                    if (mDeviceList != null) {
                        val spinnerDevice: Device = mDeviceSpiner!!.selectedItem as Device

                        val address = spinnerDevice.deviceAddress
                        val name = spinnerDevice.deviceName
                        mIdLabel?.text = name
                        mBluetoothDevice = mBluetoothAdapter!!.getRemoteDevice(address)

                        mModify = true;
                        mUpdateButton?.text = "UPDATE"
                    }
                    //mDeviceSelected = false
                }else{
                    mDeviceSelected = true
                }
            }

            override fun onNothingSelected(parentView: AdapterView<*>?) {


            }
        }

        mPrivacyPolicyLabel = findViewById<View>(R.id.privacy_policy) as TextView
        mPrivacyPolicyLabel!!.movementMethod = LinkMovementMethod.getInstance()
        mPulseCntLabel = findViewById<View>(R.id.pulsecntLabel) as TextView
        mWinTimeLabel = findViewById<View>(R.id.wtimeLabel) as TextView
        //mHpfLabel = findViewById<View>(R.id.hpfLabel) as TextView
        mThreshLabel = findViewById<View>(R.id.tresholdLabel) as TextView
        mBTimeLabel = findViewById<View>(R.id.btimeLabel) as TextView
        mTitleLabel = findViewById<View>(R.id.titleTextView) as TextView
        mIdLabel = findViewById<View>(R.id.idTextView) as TextView
        mRawLabel = findViewById<TextView>(R.id.countLabel) as TextView
        mAdvCountLabel = findViewById<TextView>(R.id.AdvCountLabel) as TextView
/*        mAdapter = DeviceListAdapter(this, R.layout.device_list_content)
        val listView =
            findViewById<View>(R.id.device_listview) as ListView
        listView.adapter = mAdapter
        listView.onItemClickListener =
            OnItemClickListener { parent, view, position, id ->
                val adapter = parent.adapter as DeviceListAdapter
                mLEScanner!!.stopScan(mScanCallback)
                val bledev = adapter.getItem(position) as BluetoothDevice
                mBleGatt = bledev.connectGatt(baseContext, false, mGattCallback)
            }
*/
        mBluetoothAdapter = bluetoothManager.adapter
        mLEScanner = mBluetoothAdapter?.bluetoothLeScanner
        mLEScanner?.startScan(mScanCallback)
/*
        mDfuBut = findViewById<View>(R.id.butDFU) as Button
        mDfuBut.setOnClickListener(View.OnClickListener {
            // Code here executes on main thread after user presses button
            val cmd = byteArrayOf(0xff.toByte())
            var res = mWrChar!!.setValue(cmd)
            res = mBleGatt!!.writeCharacteristic(mWrChar)
        })*/
    }

    private val mScanCallback: ScanCallback = object : ScanCallback() {
        override fun onScanResult(
            callbackType: Int,
            result: ScanResult
        ) {
            Log.i("callbackType", callbackType.toString())
            Log.i("result", result.toString())
            val device = result.device
            val scanRecord = result.scanRecord
            val scanData = scanRecord!!.bytes
            //val name = scanRecord.deviceName

            val address = device.address
            val name = device.name
            if (name != null) {
                if (name.equals("BluePyro")) {
                    Log.i("Devcie name: ", name)
                    val deviceID: Long = 0
                    val manuf = scanRecord.getManufacturerSpecificData(0x0177)

                    if (manuf == null)// || manuf.size < 1)
                        return

                    if (manuf.size < 1)
                        return

                    @Suppress("EXPERIMENTAL_UNSIGNED_LITERALS")
                    when (manuf[0]) {
/*                BLEADV_MANDATA_TYPE_TPH -> {
                    val press = ByteBuffer.wrap(
                        manuf,
                        1,
                        4
                    ).order(ByteOrder.LITTLE_ENDIAN).int.toDouble() / 1000.0
                    val temp = ByteBuffer.wrap(
                        manuf,
                        5,
                        2
                    ).order(ByteOrder.LITTLE_ENDIAN).short.toDouble() / 100.0
                    val humi = ByteBuffer.wrap(
                        manuf,
                        7,
                        2
                    ).order(ByteOrder.LITTLE_ENDIAN).short.toDouble() / 100.0
                    var s = String.format("%.2f", press)
                    mPressLabel.setText(s)
                    mPressLabel.getRootView().postInvalidate()
                    s = String.format("%.2f", temp)
                    mTempLabel.setText(s)
                    mTempLabel.getRootView().postInvalidate()
                    s = String.format("%.0f", humi)
                    mHumiLabel.setText(s)
                    mHumiLabel.getRootView().postInvalidate()
                }
                BLEADV_MANDATA_TYPE_GAS -> {
                    val GasRes = ByteBuffer.wrap(manuf, 1, 4)
                        .order(ByteOrder.LITTLE_ENDIAN).int
                    val AirQ = ByteBuffer.wrap(manuf, 5, 2)
                        .order(ByteOrder.LITTLE_ENDIAN).short
                    s = String.format("%d", AirQ)
                    mAirQIdxLabel.setText(s)
                }*/
                        BLEADV_MANDATA_TYPE_ADC -> {
                            val v = ByteBuffer.wrap(
                                manuf,
                                5,
                                4
                            ).order(ByteOrder.LITTLE_ENDIAN).int
                            var s = String.format("%x", v)
                            mRawLabel?.text = s

                        }
                        BLEADV_MANDATA_TYPE_APP -> {
                            val v = String(manuf, 6, manuf.size - 6, Charset.defaultCharset())
                            val name = v
                            if (name != null) {
                                if (mDeviceList!!.isEmpty()) {
                                    mDeviceList!!.add(Device(name, address))
                                } else {
                                    if (!mDeviceList!!.contains(Device(name, address))) {
                                        mDeviceList!!.add(Device(name, address))

                                    } else {
                                        mDeviceList!!.removeAt(
                                            mDeviceList!!.indexOf(
                                                Device(
                                                    name,
                                                    address
                                                )
                                            )
                                        )
                                        mDeviceList!!.add(Device(name, address))

                                    }

                                }
                                mIdLabel?.text = v
                                dataAdapter!!.notifyDataSetChanged()
                            }
                            //val v = ByteBuffer.wrap(
                            //    manuf,
                            //    1,
                            //    8
                            //).order(ByteOrder.LITTLE_ENDIAN).toString()
                            //val mdet = ByteBuffer.wrap(
                            //    manuf,
                            //    2,
                            //    8
                            //).order(ByteOrder.LITTLE_ENDIAN).ByteArray
                            //mName = manuf.copyOfRange(2, 10)

                            mAdvCnt++

                            if (manuf[1].toInt() == 0) {
                                mIdLabel?.setBackgroundColor(0xffffffff.toInt())

                            } else {
                                mDetCnt++
                                mIdLabel?.setBackgroundColor(0xffff4444.toInt())

                                var s = String.format("%d", mDetCnt)
                                mRawLabel?.text = s
                            }

                            if (mModify == false) {
                                mIdLabel?.text = v
                                var swpm = findViewById<View>(R.id.switchPulseMode) as Switch
                                val p = manuf[2].toUInt() and 0x1u
                                if (p != 0u) {
                                    // Pulse mode
                                    swpm.setChecked(true)
                                } else {
                                    swpm.setChecked(false)
                                }

                                var swhpf = findViewById<View>(R.id.switchHpf) as Switch
                                val hpf = manuf[2].toUInt() and 4u
                                if (hpf != 0u) {
                                    // HPF Cut off
                                    swhpf.setChecked(true)
                                } else {
                                    swhpf.setChecked(false)
                                }

                                var wt = (manuf[3].toInt() shr 1) and 3
                                val pc = (manuf[3].toInt() shr 3) and 3
                                val bt =
                                    ((manuf[3].toInt() shr 5) and 0x7) or ((manuf[4].toInt() and 1) shl 3)
                                val thr =
                                    ((manuf[4].toInt() shr 1) and 0x7f) or ((manuf[5].toInt() and 1) shl 7)

                                mWinTimeLabel?.text = "%d".format(wt)
                                mPulseCntLabel?.text = String.format("%d", pc)
                                mBTimeLabel?.text = String.format("%d", bt)
                                mThreshLabel?.text = String.format("%d", thr)

                                var s = String.format("%d", mAdvCnt)
                                mAdvCountLabel?.text = s
                                //s = String.format("%d", mAdvCnt)//(v shr 14) and 0x3fff)
                            }
                            //mPirLabel?.text = s
                            //s = v.toString()
                            //s = String.format("%d", v and 0x3FFF)
                            //mTempLabel?.setText(s)
                            //s = String.format("%d", v and 0x3FFF)
                            //mTempLabel?.setText(s)
                        }


                    }
                    //val listView =
                    //    findViewById<View>(R.id.device_listview) as ListView
                    //mAdapter.addItem(device)
                    //mAdapter.notifyDataSetChanged()
                    mBluetoothDevice = device
                }
            }
        }
    }
}
