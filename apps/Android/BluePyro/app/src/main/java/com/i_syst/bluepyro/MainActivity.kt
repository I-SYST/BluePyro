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
import android.widget.AdapterView

import android.widget.AdapterView.OnItemSelectedListener

import android.content.pm.PackageManager
import android.text.method.LinkMovementMethod
import androidx.annotation.RequiresPermission
import androidx.core.app.ActivityCompat


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
    var mModify : Boolean = false
    var mButtonClicked = 0
    var mDeviceSelected : Boolean = false
    private val REQUEST_BLUETOOTH_SCAN_PERMISSION = 100

    private val mGattCallback = object : BluetoothGattCallback() {
        @RequiresPermission(Manifest.permission.BLUETOOTH_CONNECT)
        override fun onConnectionStateChange(gatt: BluetoothGatt, status: Int, newState: Int) {
            if (newState == BluetoothProfile.STATE_CONNECTED) {
                gatt.discoverServices()
            } else if (newState == BluetoothProfile.STATE_DISCONNECTED) {
                // Handle disconnection
            }
        }
        @RequiresPermission(Manifest.permission.BLUETOOTH_CONNECT)
        override fun onServicesDiscovered(gatt: BluetoothGatt, status: Int) {
            if (status == BluetoothGatt.GATT_SUCCESS) {
                if (mButtonClicked == 0) {
                    val service = gatt.getService(BLUEPYRO_SERVICE_UUID)
                    mCfgChar = service.getCharacteristic(BLUEPYRO_CFGCHAR_UUID)
                    mCfgChar?.writeType = BluetoothGattCharacteristic.WRITE_TYPE_DEFAULT

                    val tresh = Integer.valueOf(mThreshLabel?.text.toString()) and 0xff
                    val btime = Integer.valueOf(mBTimeLabel?.text.toString()) and 0xf
                    val wtime = Integer.valueOf(mWinTimeLabel?.text.toString()) and 3
                    val hpf =
                        if ((findViewById<View>(R.id.switchHpf) as Switch).isChecked()) 1 else 0
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
            } else {
                // Service discovery failed
            }
        }
        // ... (rest of onCharacteristicRead, onCharacteristicWrite, onCharacteristicChanged)
        override fun onCharacteristicRead(gatt: BluetoothGatt, characteristic: BluetoothGattCharacteristic, status: Int) {
            val values = characteristic.value
            val clearValue = byteArrayOf(255.toByte())
            var value = 0
            if (null != values) {
            }
            if (status == BluetoothGatt.GATT_SUCCESS) {
            } else {
            }
        }
        @RequiresPermission(Manifest.permission.BLUETOOTH_CONNECT)
        override fun onCharacteristicWrite(gatt: BluetoothGatt, characteristic: BluetoothGattCharacteristic, status: Int) {
            if (status == BluetoothGatt.GATT_SUCCESS) {
                mBleGatt?.disconnect()
                mModify = false
            }
        }
        override fun onCharacteristicChanged(gatt: BluetoothGatt, characteristic: BluetoothGattCharacteristic?) {
            if (characteristic != null && characteristic.properties == BluetoothGattCharacteristic.PROPERTY_NOTIFY) {
            }
            if (characteristic != null) {
            }
        }
    }

    @RequiresPermission(Manifest.permission.BLUETOOTH_SCAN)
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        // Request runtime permissions for ACCESS_FINE_LOCATION and BLUETOOTH_SCAN
        // For Android 12+ (API 31+), BLUETOOTH_SCAN and BLUETOOTH_CONNECT are needed
        if (checkSelfPermission(Manifest.permission.BLUETOOTH_SCAN) != PackageManager.PERMISSION_GRANTED) {
            ActivityCompat.requestPermissions(this,
                arrayOf(Manifest.permission.ACCESS_FINE_LOCATION, Manifest.permission.BLUETOOTH_SCAN, Manifest.permission.BLUETOOTH_CONNECT),
                REQUEST_BLUETOOTH_SCAN_PERMISSION)
        } else {
            // Permissions are already granted, proceed with Bluetooth setup
            setupBluetooth()
        }
    }

    @RequiresPermission(Manifest.permission.BLUETOOTH_SCAN)
    override fun onRequestPermissionsResult(requestCode: Int, permissions: Array<out String>, grantResults: IntArray) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults)
        if (requestCode == REQUEST_BLUETOOTH_SCAN_PERMISSION) {
            if (grantResults.isNotEmpty() && grantResults[0] == PackageManager.PERMISSION_GRANTED) {
                // Permission granted, proceed with Bluetooth setup
                setupBluetooth()
            } else {
                // Permission denied, handle gracefully (e.g., show a message and disable functionality)
                Log.e("Permissions", "Bluetooth scan permission denied.")
            }
        }
    }

    @RequiresPermission(Manifest.permission.BLUETOOTH_SCAN)
    private fun setupBluetooth() {
        mCtx = baseContext
        val bluetoothManager = getSystemService(BLUETOOTH_SERVICE) as BluetoothManager

        mUpdateButton = findViewById<View>(R.id.button) as Button
        mUpdateButton?.setOnClickListener {
            if (mModify) {
                mButtonClicked = 0
                if (mBluetoothDevice != null) {
                    if (ActivityCompat.checkSelfPermission(this, Manifest.permission.BLUETOOTH_CONNECT) != PackageManager.PERMISSION_GRANTED) {
                        return@setOnClickListener
                    }
                    mBleGatt = mBluetoothDevice?.connectGatt(mCtx, false, mGattCallback)
                }
                mUpdateButton?.text = "MODIFY"
            } else {
                mModify = true
                mUpdateButton?.text = "UPDATE"
            }
        }

        mDFUButton = findViewById<View>(R.id.dfu) as Button
        mDFUButton?.setOnClickListener @androidx.annotation.RequiresPermission(android.Manifest.permission.BLUETOOTH_CONNECT) {
            mButtonClicked = 1
            if (mBluetoothDevice != null) {
                if (ActivityCompat.checkSelfPermission(this, Manifest.permission.BLUETOOTH_CONNECT) == PackageManager.PERMISSION_GRANTED) {
                    mBleGatt = mBluetoothDevice?.connectGatt(mCtx, false, mGattCallback)
                }
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
                if (mDeviceSelected) {
                    if (mDeviceList != null) {
                        val spinnerDevice: Device = mDeviceSpiner!!.selectedItem as Device
                        val address = spinnerDevice.deviceAddress
                        val name = spinnerDevice.deviceName
                        mIdLabel?.text = name
                        mBluetoothDevice = mBluetoothAdapter!!.getRemoteDevice(address)
                        mModify = true
                        mUpdateButton?.text = "UPDATE"
                    }
                } else {
                    mDeviceSelected = true
                }
            }

            override fun onNothingSelected(parentView: AdapterView<*>?) {}
        }

        mPrivacyPolicyLabel = findViewById<View>(R.id.privacy_policy) as TextView
        mPrivacyPolicyLabel!!.movementMethod = LinkMovementMethod.getInstance()
        mPulseCntLabel = findViewById<View>(R.id.pulsecntLabel) as TextView
        mWinTimeLabel = findViewById<View>(R.id.wtimeLabel) as TextView
        mThreshLabel = findViewById<View>(R.id.tresholdLabel) as TextView
        mBTimeLabel = findViewById<View>(R.id.btimeLabel) as TextView
        mTitleLabel = findViewById<View>(R.id.titleTextView) as TextView
        mIdLabel = findViewById<View>(R.id.idTextView) as TextView
        mRawLabel = findViewById<TextView>(R.id.countLabel) as TextView
        mAdvCountLabel = findViewById<TextView>(R.id.AdvCountLabel) as TextView

        mBluetoothAdapter = bluetoothManager.adapter
        mLEScanner = mBluetoothAdapter?.bluetoothLeScanner

        // ADD THIS CHECK:
        if (ActivityCompat.checkSelfPermission(this, Manifest.permission.BLUETOOTH_SCAN) == PackageManager.PERMISSION_GRANTED) {
            mLEScanner?.startScan(mScanCallback)
        } else {
            // Handle the case where permission is still not granted (should not happen, but is good practice)
            Log.e("Permissions", "Failed to start scan: BLUETOOTH_SCAN permission not granted.")
        }
    }

    private val mScanCallback: ScanCallback = object : ScanCallback() {
        @RequiresPermission(Manifest.permission.BLUETOOTH_CONNECT)
        override fun onScanResult(
            callbackType: Int,
            result: ScanResult
        ) {
            Log.i("callbackType", callbackType.toString())
            Log.i("result", result.toString())
            val device = result.device
            val scanRecord = result.scanRecord
            val scanData = scanRecord!!.bytes
            val address = device.address
            val name = device.name
            if (name != null) {
                if (name.equals("BluePyro")) {
                    Log.i("Devcie name: ", name)
                    val deviceID: Long = 0
                    val manuf = scanRecord.getManufacturerSpecificData(0x0177)

                    if (manuf == null || manuf.size < 1)
                        return

                    @Suppress("EXPERIMENTAL_UNSIGNED_LITERALS")
                    when (manuf[0]) {
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
                                    swpm.setChecked(true)
                                } else {
                                    swpm.setChecked(false)
                                }

                                var swhpf = findViewById<View>(R.id.switchHpf) as Switch
                                val hpf = manuf[2].toUInt() and 4u
                                if (hpf != 0u) {
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
                            }
                            mBluetoothDevice = device
                        }
                    }
                }
            }
        }
    }
}