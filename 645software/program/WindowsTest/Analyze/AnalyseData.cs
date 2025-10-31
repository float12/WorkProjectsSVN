using System;

namespace WindowsTest
{
    class AnalyseData
    {
        private static object NW_DLLObject = new object();
        /// <summary>
        /// 服务器调用加密机方法
        /// </summary>
        /// <param name="bRecv">接收数据(格式:库号+方法号+公私钥+数据+长度)</param>
        /// <param name="strOutData"></param>
        /// <returns></returns>
        //public string fnGetDataFromEncrptyMahine(string bRecv,string point,ref string strOutData)
        //{
        //    string strRecv = "";
        //    string strRet = "";
        //    string strMethod = "";
        //    string strClassType = "";//调用动态库//00:国网-正式版; 01:国网-厂家版; 10:南网-南瑞智芯; 11:南网-南瑞交互终端; 12:南网-融通,30
        //    string strData = "";
        //    string[] arrRecv=new string[]{};
        //    int iFlag = -1;
        //    try
        //    {
        //        strRecv =bRecv.Substring(0,bRecv.Length-2);
        //        arrRecv = strRecv.Split(',');
        //        strClassType = arrRecv[0];//.ToString("X2")
        //        strMethod = arrRecv[1];
        //        iFlag = int.Parse(arrRecv[2]);//公私钥(0公钥1私钥)
        //        //strData = arrRecv[3];

        //    }
        //    catch(Exception ex){
        //        strRet = "aa";
        //    }
        //    if (strClassType == "00")
        //    {
        //        StaticMTEproperty.isTestDll = true;
        //    }
        //    if (strClassType == "01")
        //    {
        //        StaticMTEproperty.isTestDll = false;
        //    }
        //    if (strClassType == "00" || strClassType == "01")
        //    {
        //        lock (NW_DLLObject)
        //        {
        //            #region 国网-正式版动态库
        //            switch (strMethod)
        //            {
        //                case "00":
        //                    #region 身份认证
        //                    try
        //                    {
        //                        strRet = ICCard_Remote.fnIdentityAuthentication(iFlag, arrRecv[3], ref strOutData);
        //                    }
        //                    catch
        //                    {
        //                        strRet = "F0";
        //                    }
        //                    #endregion
        //                    break;
        //                case "01":
        //                    #region 82文件
        //                    try
        //                    {
        //                        strRet = ICCard_Remote.fnUpdateParameter(iFlag, arrRecv[3], arrRecv[4], arrRecv[5], arrRecv[6], ref strOutData);
        //                    }
        //                    catch
        //                    {
        //                        strRet = "F1";
        //                    }
        //                    #endregion
        //                    break;
        //                case "02":
        //                    #region 83文件
        //                    try
        //                    {
        //                        strRet = ICCard_Remote.fnUpdateTarrif1(iFlag, arrRecv[3], arrRecv[4], arrRecv[5], arrRecv[6], ref strOutData);
        //                    }
        //                    catch
        //                    {
        //                        strRet = "F2";
        //                    }
        //                    #endregion
        //                    break;
        //                case "03":
        //                    #region 84文件
        //                    try
        //                    {
        //                        strRet = ICCard_Remote.fnUpdateTarrif2(iFlag, arrRecv[3], arrRecv[4], arrRecv[5], arrRecv[6], ref strOutData);

        //                    }
        //                    catch
        //                    {
        //                        strRet = "F3";
        //                    }
        //                    #endregion
        //                    break;
        //                case "04":
        //                    #region 98级写
        //                    try
        //                    {
        //                        strRet = ICCard_Remote.fnUpdateParameterCipherMAC(iFlag, arrRecv[3], arrRecv[4], arrRecv[5], arrRecv[6], ref strOutData);
        //                    }
        //                    catch
        //                    {
        //                        strRet = "F4";
        //                    }
        //                    #endregion
        //                    break;
        //                case "05":
        //                    #region 控制类
        //                    try
        //                    {
        //                        strRet = ICCard_Remote.fnUserControl(iFlag, arrRecv[3], arrRecv[4], arrRecv[5], arrRecv[6], ref strOutData);

        //                    }
        //                    catch
        //                    {
        //                        strRet = "F5";
        //                    }
        //                    #endregion
        //                    break;
        //                case "06":
        //                    #region 电表清零
        //                    try
        //                    {
        //                        //string strRandom2 = strData.Substring(0, 8);
        //                        //string strDiv = strData.Substring(8, 16);
        //                        //string strMingWen = strData.Substring(24);
        //                        strRet = ICCard_Remote.DataClear1(iFlag, arrRecv[3], arrRecv[4], arrRecv[5], ref strOutData);

        //                    }
        //                    catch
        //                    {
        //                        strRet = "F6";
        //                    }
        //                    #endregion
        //                    break;
        //                case "07":
        //                    #region 需量清零/事件清零
        //                    try
        //                    {
        //                        //string strRandom2 = strData.Substring(0, 8);
        //                        //string strDiv = strData.Substring(8, 16);
        //                        //string strMingWen = strData.Substring(24);
        //                        strRet = ICCard_Remote.DataClear2(iFlag, arrRecv[3], arrRecv[4], arrRecv[5], ref strOutData);

        //                    }
        //                    catch
        //                    {
        //                        strRet = "F7";
        //                    }
        //                    #endregion
        //                    break;
        //                case "08":
        //                    #region 钱包初始化
        //                    try
        //                    {
        //                        //string strRandom2 = strData.Substring(0, 8);
        //                        //string strDiv = strData.Substring(8, 16);
        //                        //string strRandDivData = strData.Substring(24);
        //                        strRet = ICCard_Remote.fnInintMoney(iFlag, arrRecv[3], arrRecv[4], arrRecv[5], ref strOutData);

        //                    }
        //                    catch
        //                    {
        //                        strRet = "F8";
        //                    }
        //                    #endregion
        //                    break;
        //                case "09":
        //                    #region 开户/充值
        //                    try
        //                    {
        //                        //string strRandom2 = strData.Substring(0, 8);
        //                        //string strDiv = strData.Substring(8, 16);
        //                        //string strRandDivData = strData.Substring(24);
        //                        strRet = ICCard_Remote.fnAddMoney(iFlag, arrRecv[3], arrRecv[4], arrRecv[5], ref strOutData);

        //                    }
        //                    catch
        //                    {
        //                        strRet = "F9";
        //                    }
        //                    #endregion
        //                    break;
        //                case "10":
        //                    #region 退费
        //                    try
        //                    {
        //                        //string strRandom2 = strData.Substring(0, 8);
        //                        //string strDiv = strData.Substring(8, 16);
        //                        //string strRandDivData = strData.Substring(24);
        //                        strRet = ICCard_Remote.fnDecreaseMoney(iFlag, arrRecv[3], arrRecv[4], arrRecv[5], ref strOutData);

        //                    }
        //                    catch
        //                    {
        //                        strRet = "FA";
        //                    }
        //                    #endregion
        //                    break;
        //                case "11":
        //                    #region 密钥更新
        //                    try
        //                    {
        //                        //int iKeyNum = int.Parse(strData.Substring(0, 2));
        //                        //string strPutKeystate = strData.Substring(2, 2);//密钥状态，“00”，密钥恢复；“01”，密钥下装；
        //                        //string strPutKeyid = strData.Substring(4, 8);
        //                        //string strRandom2 = strData.Substring(12, 8);
        //                        //string strDiv = strData.Substring(20, 16);
        //                        //string strESAMNo = strData.Substring(36, 16);
        //                        //string strChipInfor = strData.Substring(52);
        //                        strRet = ICCard_Remote.fnUpdateKey(int.Parse(arrRecv[2]),arrRecv[3], arrRecv[4], arrRecv[5], arrRecv[6], arrRecv[7], arrRecv[8], ref strOutData);

        //                    }
        //                    catch(Exception ex)
        //                    {
        //                        strRet = "FB";
        //                    }
        //                    #endregion
        //                    break;
        //                case "12":
        //                    #region MAC校验
        //                    try
        //                    {
        //                        //string strRandom1 = strData.Substring(0, 8);
        //                        //string strDiv = strData.Substring(8, 16);
        //                        //string strPutApdu = strData.Substring(24, 10);
        //                        //string strInput = strData.Substring(34, strData.Length - 34 - 8);
        //                        //string strMac = strData.Substring(strData.Length - 8);
        //                        strRet = ICCard_Remote.fnCheckMAC(iFlag, arrRecv[3], arrRecv[4], arrRecv[5], arrRecv[6], arrRecv[7]);

        //                    }
        //                    catch
        //                    {
        //                        strRet = "FC";
        //                    }
        //                    #endregion
        //                    break;
        //                case "13":
        //                    #region 红外认证-获取随机数
        //                    try
        //                    {
        //                        strRet = ICCard_Remote.InfraredGetRand(ref strOutData);
        //                    }
        //                    catch
        //                    {
        //                        strRet = "FD";
        //                    }
        //                    #endregion
        //                    break;
        //                case "14":
        //                    #region 红外认证
        //                    try
        //                    {
        //                        //string strDiv = strData.Substring(0, 16);
        //                        //string strESAMNo = strData.Substring(16, 16);//ESAM序列号
        //                        //string strRandom1 = strData.Substring(32, 16);//随机数1
        //                        //string strPSW1 = strData.Substring(48, 16);//密文1
        //                        //string strPSW2 = strData.Substring(64, 16);//随机数2
        //                        strRet = ICCard_Remote.InfraredAuthFlag(iFlag, arrRecv[3], arrRecv[4], arrRecv[5], arrRecv[6], arrRecv[7], ref strOutData);

        //                    }
        //                    catch
        //                    {
        //                        strRet = "FE";
        //                    }
        //                    #endregion
        //                    break;
        //                case "80":
        //                    #region 发行用户卡

        //                    #endregion
        //                    break;
        //                case "81":
        //                    #region 读取用户卡

        //                    #endregion
        //                    break;
        //                default:
        //                    break;
        //            }
        //            #endregion
        //        }
        //     }
        //    else if (strClassType == "10")
        //    {
        //        lock (NW_DLLObject)
        //        {
        //            #region 南网-南瑞
        //            switch (strMethod)
        //            {
        //                case "00":
        //                    #region 身份认证
        //                    try
        //                    {
        //                        strRet = ICCardLib_2015_NW_NRZX.fnIdentityAuthentication(iFlag, arrRecv[3], ref strOutData);
        //                    }
        //                    catch
        //                    {
        //                        strRet = "F0";
        //                    }
        //                    #endregion
        //                    break;
        //                case "01":
        //                    #region 82文件
        //                    try
        //                    {
        //                        //string strRandom2 = strData.Substring(0, 8);
        //                        //string strDiv = strData.Substring(8, 16);
        //                        //string strParamUpdateCmd = strData.Substring(24, 10);
        //                        //string strValues = strData.Substring(34);
        //                        strRet = ICCardLib_2015_NW_NRZX.fnParameterUpdate(iFlag, arrRecv[3], arrRecv[4], arrRecv[5], arrRecv[6], ref strOutData);
        //                    }
        //                    catch
        //                    {
        //                        strRet = "F1";
        //                    }
        //                    #endregion
        //                    break;
        //                case "02":
        //                    #region 83文件
        //                    try
        //                    {
        //                        //string strRandom2 = strData.Substring(0, 8);
        //                        //string strDiv = strData.Substring(8, 16);
        //                        //string strParamUpdateCmd = strData.Substring(24, 10);
        //                        //string strValues = strData.Substring(34);
        //                        strRet = ICCardLib_2015_NW_NRZX.fnPrice1Update(iFlag, arrRecv[3], arrRecv[4], arrRecv[5], arrRecv[6], ref strOutData);
        //                    }
        //                    catch
        //                    {
        //                        strRet = "F2";
        //                    }
        //                    #endregion
        //                    break;
        //                case "03":
        //                    #region 84文件
        //                    try
        //                    {
        //                        //string strRandom2 = strData.Substring(0, 8);
        //                        //string strDiv = strData.Substring(8, 16);
        //                        //string strParamUpdateCmd = strData.Substring(24, 10);
        //                        //string strValues = strData.Substring(34);
        //                        strRet = ICCardLib_2015_NW_NRZX.fnPrice2Update(iFlag, arrRecv[3], arrRecv[4], arrRecv[5], arrRecv[6], ref strOutData);

        //                    }
        //                    catch
        //                    {
        //                        strRet = "F3";
        //                    }
        //                    #endregion
        //                    break;
        //                case "04":
        //                    #region 98级写
        //                    try
        //                    {
        //                        //string strRandom2 = strData.Substring(0, 8);
        //                        //string strDiv = strData.Substring(8, 16);
        //                        //string strParamUpdateCmd = strData.Substring(24, 10);
        //                        //string strRandApduDivData = strData.Substring(34);
        //                        strRet = ICCardLib_2015_NW_NRZX.fnParameterElseUpdate(iFlag, arrRecv[3], arrRecv[4], arrRecv[5], arrRecv[6], ref strOutData);

        //                    }
        //                    catch
        //                    {
        //                        strRet = "F4";
        //                    }
        //                    #endregion
        //                    break;
        //                case "05":
        //                    #region 控制类
        //                    try
        //                    {
        //                        //string strRandom2 = strData.Substring(0, 8);
        //                        //string strDiv = strData.Substring(8, 16);
        //                        //string strESAMNo = strData.Substring(24, 16);
        //                        //string strMingWen = strData.Substring(40);
        //                        strRet = ICCardLib_2015_NW_NRZX.fnUserControl(iFlag, arrRecv[3], arrRecv[4], arrRecv[5], arrRecv[6], ref strOutData);

        //                    }
        //                    catch
        //                    {
        //                        strRet = "F5";
        //                    }
        //                    #endregion
        //                    break;
        //                case "06":
        //                    #region 需量清零/电表清零
        //                    try
        //                    {
        //                        //string strRandom2 = strData.Substring(0, 8);
        //                        //string strDiv = strData.Substring(8, 16);
        //                        //string strMingWen = strData.Substring(24);
        //                        strRet = ICCardLib_2015_NW_NRZX.fnDataClear1(iFlag, arrRecv[3], arrRecv[4], arrRecv[5], ref strOutData);

        //                    }
        //                    catch
        //                    {
        //                        strRet = "F6";
        //                    }
        //                    #endregion
        //                    break;
        //                case "07":
        //                    #region 事件清零
        //                    try
        //                    {
        //                        //string strRandom2 = strData.Substring(0, 8);
        //                        //string strDiv = strData.Substring(8, 16);
        //                        //string strMingWen = strData.Substring(24);
        //                        strRet = ICCardLib_2015_NW_NRZX.fnDataClear1(iFlag, arrRecv[3], arrRecv[4], arrRecv[5], ref strOutData);

        //                    }
        //                    catch
        //                    {
        //                        strRet = "F7";
        //                    }
        //                    #endregion
        //                    break;
        //                case "08":
        //                    #region 钱包初始化
        //                    try
        //                    {
        //                        //string strRandom2 = strData.Substring(0, 8);
        //                        //string strDiv = strData.Substring(8, 16);
        //                        //string strMoney = strData.Substring(24);
        //                        strRet = ICCardLib_2015_NW_NRZX.fnInitPurse(iFlag, arrRecv[3], arrRecv[4], arrRecv[5], ref strOutData);

        //                    }
        //                    catch
        //                    {
        //                        strRet = "F8";
        //                    }
        //                    #endregion
        //                    break;
        //                case "09":
        //                    #region 开户/充值
        //                    try
        //                    {
        //                        //string strRandom2 = strData.Substring(0, 8);
        //                        //string strDiv = strData.Substring(8, 16);
        //                        //string strRandDivData = strData.Substring(24);
        //                        strRet = ICCardLib_2015_NW_NRZX.fnIncreasePurse(iFlag, arrRecv[3], arrRecv[4], arrRecv[5], ref strOutData);
        //                    }
        //                    catch
        //                    {
        //                        strRet = "F9";
        //                    }
        //                    #endregion
        //                    break;
        //                case "10":
        //                    #region 退费
        //                    try
        //                    {
        //                        //string strRandom2 = strData.Substring(0, 8);
        //                        //string strDiv = strData.Substring(8, 16);
        //                        //string strRandDivData = strData.Substring(24);
        //                        strRet = ICCardLib_2015_NW_NRZX.fnDecreasePurse(iFlag, arrRecv[3], arrRecv[4], arrRecv[5], ref strOutData);

        //                    }
        //                    catch
        //                    {
        //                        strRet = "FA";
        //                    }
        //                    #endregion
        //                    break;
        //                case "11":
        //                    #region 密钥更新
        //                    try
        //                    {
        //                        if (strData.Length < 52)
        //                        {

        //                        }
        //                        //int iKeyNum = int.Parse(strData.Substring(0, 2));
        //                        //string strPutKeystate = strData.Substring(2, 2);//密钥状态，“00”，密钥恢复；“01”，密钥下装；
        //                        //string strPutKeyid = strData.Substring(4, strData.Length - 4 - 8 - 16 - 16);
        //                        //string strRandom2 = strData.Substring(strPutKeyid.Length + 4, 8);
        //                        //string strDiv = strData.Substring(strPutKeyid.Length + 4 + 8, 16);
        //                        //string strESAMNo = strData.Substring(strPutKeyid.Length + 4 + 8 + 16, 16);
        //                        strRet = ICCardLib_2015_NW_NRZX.fnKeyUpdateV2(int.Parse(arrRecv[3]), arrRecv[4], arrRecv[5], arrRecv[6], arrRecv[7], arrRecv[8], ref strOutData);

        //                    }
        //                    catch
        //                    {
        //                        strRet = "FB";
        //                    }
        //                    #endregion
        //                    break;
        //                case "12":
        //                    #region MAC校验
        //                    try
        //                    {
        //                        //string strRandom1 = strData.Substring(0, 8);
        //                        //string strDiv = strData.Substring(8, 16);
        //                        //string strPutApdu = strData.Substring(24, 10);
        //                        //string strInput = strData.Substring(34, strData.Length - 34 - 8);
        //                        //string strMac = strData.Substring(strData.Length - 8);
        //                        strRet = ICCardLib_2015_NW_NRZX.fnMacCheck(iFlag, arrRecv[3], arrRecv[4], arrRecv[5], arrRecv[6], arrRecv[7]);

        //                    }
        //                    catch
        //                    {
        //                        strRet = "FC";
        //                    }
        //                    #endregion
        //                    break;
        //                case "13":
        //                    #region 红外认证-获取随机数
        //                    try
        //                    {
        //                        strRet = ICCardLib_2015_NW_NRZX.fnInfraredRand(ref strOutData);
        //                    }
        //                    catch
        //                    {
        //                        strRet = "FD";
        //                    }
        //                    #endregion
        //                    break;
        //                case "14":
        //                    #region 红外认证
        //                    try
        //                    {
        //                        //string strDiv = strData.Substring(0, 16);
        //                        //string strESAMNo = strData.Substring(16, 16);//ESAM序列号
        //                        //string strRandom1 = strData.Substring(32, 16);//随机数1
        //                        //string strPSW1 = strData.Substring(48, 16);//密文1
        //                        //string strPSW2 = strData.Substring(64, 16);//随机数2
        //                        strRet = ICCardLib_2015_NW_NRZX.fnInfraredAuth(iFlag, arrRecv[3], arrRecv[4], arrRecv[5], arrRecv[6], arrRecv[7], ref strOutData);

        //                    }
        //                    catch
        //                    {
        //                        strRet = "FE";
        //                    }
        //                    #endregion
        //                    break;
        //                case "15":
        //                    #region 比对(南网)
        //                    try
        //                    {
        //                        //string strKeyIndex = strData.Substring(0, 2);
        //                        //string strCompareDiv = strData.Substring(2, 16);
        //                        //string strCompareData = strData.Substring(18);
        //                        strRet = ICCardLib_2015_NW_NRZX.fnEncForCompare(arrRecv[3], arrRecv[4], arrRecv[5], ref strOutData);
        //                        strOutData = strOutData.Substring(0, 128);
        //                    }
        //                    catch (System.Exception ex)
        //                    {
        //                        strRet = "E1";
        //                    }
        //                    #endregion
        //                    break;
        //                case "16":
        //                    #region 费控模式切换
        //                    try
        //                    {
        //                        //string strRandom2 = strData.Substring(0, 8);
        //                        //string strDiv = strData.Substring(8, 16);
        //                        //string strPutData = strData.Substring(24);
        //                        strRet = ICCardLib_2015_NW_NRZX.fnSwitchChargeMode(iFlag, arrRecv[3], arrRecv[4], arrRecv[5], ref strOutData);
        //                    }
        //                    catch (System.Exception ex)
        //                    {
        //                        strRet = "E2";
        //                    }
        //                    #endregion
        //                    break;
        //                case "17":
        //                    #region 记录信息文件 1 更新
        //                    try
        //                    {
        //                        //string strRandom2 = strData.Substring(0, 8);
        //                        //string strDiv = strData.Substring(8, 16);
        //                        //string strESAMNo = strData.Substring(24, 16);//ESAM序列号
        //                        //string strPutFileID = strData.Substring(40, 2);//1 字节文件标识
        //                        //string strPutDataBegin = strData.Substring(42, 4); //2 字节起始字节
        //                        //string strPutData = strData.Substring(46); //明文数据，文件最大 0x95 字节
        //                        strRet = ICCardLib_2015_NW_NRZX.fnMacWrite(iFlag, arrRecv[3], arrRecv[4], arrRecv[5], arrRecv[6], arrRecv[7], arrRecv[8], ref strOutData);
        //                    }
        //                    catch (System.Exception ex)
        //                    {
        //                        strRet = "E3";
        //                    }
        //                    #endregion
        //                    break;
        //                case "18":
        //                    #region 记录信息文件 2 更新
        //                    try
        //                    {
        //                        //string strRandom2 = strData.Substring(0, 8);
        //                        //string strDiv = strData.Substring(8, 16);
        //                        //string strESAMNo = strData.Substring(24, 16);//ESAM序列号
        //                        //string strPutFileID = strData.Substring(40, 2);//1 字节文件标识
        //                        //string strPutDataBegin = strData.Substring(42, 4); //2 字节起始字节
        //                        //string strPutData = strData.Substring(46); //明文数据，文件最大 0x95 字节
        //                        strRet = ICCardLib_2015_NW_NRZX.fnEncMacWrite(iFlag, arrRecv[3], arrRecv[4], arrRecv[5], arrRecv[6], arrRecv[7], arrRecv[8], ref strOutData);
        //                    }
        //                    catch (System.Exception ex)
        //                    {
        //                        strRet = "E4";
        //                    }
        //                    #endregion
        //                    break;
        //                case "19":
        //                    #region 交互终端身份认证
        //                    try
        //                    {
        //                        strRet = ICCard_InteractiveTerminal_NW_NRZX.fnIdentityAuthentication3(iFlag, arrRecv[3], ref strOutData);
        //                    }
        //                    catch (System.Exception ex)
        //                    {
        //                        strRet = "E5";
        //                    }
        //                    #endregion
        //                    break;
        //                case "20":
        //                    #region 读卡片参数信息
        //                    try
        //                    {
        //                        string OutCardNumber = "";//卡片序列号 8 字节
        //                        string OutRand = "";//随机数 4 字节
        //                        string OutCardType = "";//电卡类型 1 字节, 值同参数文件命令码
        //                        string OutWalletCash = "";//钱包文件金额 4 字节， HEX 码
        //                        string OutWalletCount = "";//钱包文件次数 4 字节， HEX 码
        //                        string OutMAC1 = "";//4 字节， HEX 码
        //                        string OutMAC2 = "";//4 字节， HEX 码
        //                        string OutParaFile = "";//参数信息文件(完整文件)
        //                        string OutMAC3 = "";//4 字节， HEX 码
        //                        string OutMAC4 = "";//4 字节， HEX 码
        //                        strRet = ICCard_InteractiveTerminal_NW_NRZX.fnReadParaFile(arrRecv[3], ref OutCardNumber, ref OutRand, ref OutCardType, ref OutWalletCash, ref OutWalletCount, ref OutMAC1, ref OutMAC2, ref OutParaFile, ref OutMAC3, ref OutMAC4);
        //                        strOutData = OutCardNumber + "," + OutRand + "," + OutCardType + "," + OutWalletCash + "," + OutWalletCount + "," + OutMAC1 + "," + OutMAC2 + "," + OutParaFile + "," + OutMAC3 + "," + OutMAC4;
        //                    }
        //                    catch (System.Exception ex)
        //                    {
        //                        strRet = "E6";
        //                    }
        //                    #endregion
        //                    break;
        //                case "21":
        //                    #region 读当前套电价文件
        //                    try
        //                    {
        //                        string strOutPriceFile1 = "";
        //                        string strOutMAC = "";
        //                        strRet = ICCard_InteractiveTerminal_NW_NRZX.fnReadPriceFile1(arrRecv[3], ref strOutPriceFile1, ref strOutMAC);
        //                        strOutData = strOutPriceFile1 + "," + strOutMAC;
        //                    }
        //                    catch (System.Exception ex)
        //                    {
        //                        strRet = "E7";
        //                    }
        //                    #endregion
        //                    break;
        //                case "22":
        //                    #region 读备用套电价文件
        //                    try
        //                    {
        //                        string strOutPriceFile1 = "";
        //                        string strOutMAC = "";
        //                        strRet = ICCard_InteractiveTerminal_NW_NRZX.fnReadPriceFile2(arrRecv[3], ref strOutPriceFile1, ref strOutMAC);
        //                        strOutData = strOutPriceFile1 + "," + strOutMAC;

        //                    }
        //                    catch (System.Exception ex)
        //                    {
        //                        strRet = "E8";
        //                    }
        //                    #endregion
        //                    break;
        //                case "23":
        //                    #region 读用户卡合闸复电命令文件
        //                    try
        //                    {
        //                        strRet = ICCard_InteractiveTerminal_NW_NRZX.fnReadControlCommandFromUserCard(arrRecv[3], ref strOutData);
        //                    }
        //                    catch (System.Exception ex)
        //                    {
        //                        strRet = "E9";
        //                    }
        //                    #endregion
        //                    break;
        //                case "24":
        //                    #region 写用户卡返写信息文件
        //                    try
        //                    {
        //                        strRet = ICCard_InteractiveTerminal_NW_NRZX.fnWriteReplyFileToUserCard(arrRecv[3]);
        //                        strOutData = strRet;
        //                    }
        //                    catch (System.Exception ex)
        //                    {
        //                        strRet = "EA";
        //                    }
        //                    #endregion
        //                    break;
        //                default:
        //                    break;
        //            }
        //            #endregion
        //        }
        //    }
        //    else if (strClassType == "30")
        //    {
        //        lock (NW_DLLObject)
        //        {
        //            #region 698加密协议
        //            switch (strMethod)
        //            {
        //                case "30":
        //                    #region 主站会话协商
        //                    try
        //                    {
        //                        string sOutRandHost="";
        //                        string sOutSessionInit="";
        //                        string sOutSign="";

        //                        strRet = Lib_GuoWang698_2016.fnObj_Meter_Formal_InitSession(iFlag, arrRecv[3], arrRecv[4], arrRecv[5], ref sOutRandHost,ref sOutSessionInit,ref sOutSign);
        //                        strOutData = sOutRandHost+ "," + sOutSessionInit + "," + sOutSign;
        //                    }
        //                    catch
        //                    {
        //                        strRet = "F0";
        //                    }
        //                    #endregion
        //                    break;
        //                case "31":
        //                    #region 主站会话协商验证
        //                    try
        //                    {
        //                        strRet = Lib_GuoWang698_2016.fnObj_Meter_Formal_VerifySession(iFlag, arrRecv[3], arrRecv[4], arrRecv[5], arrRecv[6],ref strOutData);
        //                    }
        //                    catch
        //                    {
        //                        strRet = "F1";
        //                    }
        //                    #endregion
        //                    break;
        //                case "32":
        //                    #region 抄读数据验证
        //                    try
        //                    {
                                
        //                        strRet = Lib_GuoWang698_2016.fnObj_Meter_Formal_VerifyReadData(iFlag, int.Parse(arrRecv[3]), arrRecv[4], arrRecv[5], arrRecv[6], arrRecv[7], ref strOutData);
                               
        //                    }
        //                    catch
        //                    {
        //                        strRet = "F3";
        //                    }
        //                    #endregion
        //                    break;
        //                case "33":
        //                    #region 上报数据验证
        //                    try
        //                    {
        //                        string sOutData = "";
        //                        string sOutRSTCTR="";
        //                        strRet = Lib_GuoWang698_2016.fnObj_Meter_Formal_VerifyReportData(iFlag, int.Parse(arrRecv[3]), arrRecv[4], arrRecv[5], arrRecv[6], arrRecv[7], ref sOutData, ref sOutRSTCTR);
        //                        strOutData = sOutData + "," + sOutRSTCTR;
        //                    }
        //                    catch
        //                    {
        //                        strRet = "F4";
        //                    }
        //                    #endregion
        //                    break;
        //                case "34":
        //                    #region 上报数据返回报文加密
        //                    try
        //                    {   
        //                        string sOutSID ="";
        //                        string sOutAttachData = "";
        //                        string sOutData = "";
        //                        string sOutMac = "";
        //                        strRet = Lib_GuoWang698_2016.fnObj_Meter_Formal_GetResponseData(iFlag, int.Parse(arrRecv[3]), arrRecv[4], arrRecv[5], arrRecv[6], ref sOutSID, ref sOutAttachData, ref sOutData, ref sOutMac);
        //                        strOutData = sOutSID + "," + sOutAttachData + "," + sOutData+ "," + sOutMac;
        //                    }
        //                    catch
        //                    {
        //                        strRet = "F5";
        //                    }
        //                    #endregion
        //                    break;
        //                case "35":
        //                    #region 安全传输加密
        //                    try
        //                    {
        //                        string sOutSID = "";
        //                        string sOutAttachData = "";
        //                        string sOutData = "";
        //                        string sOutMac = "";
        //                        strRet = Lib_GuoWang698_2016.fnObj_Meter_Formal_GetSessionData(int.Parse(arrRecv[2]), arrRecv[3], arrRecv[4], int.Parse(arrRecv[5]),arrRecv[6], ref sOutSID, ref sOutAttachData, ref sOutData, ref sOutMac);
        //                        strOutData = sOutSID + "," + sOutAttachData + "," + sOutData + "," + sOutMac;
        //                    }
        //                    catch
        //                    {
        //                        strRet = "F6";
        //                    }
        //                    #endregion
        //                    break;
        //                case "36":
        //                    #region 安全传输解密
        //                    try
        //                    {
        //                        strRet = Lib_GuoWang698_2016.fnObj_Meter_Formal_VerifyMeterData(iFlag, int.Parse(arrRecv[3]), arrRecv[4], arrRecv[5], arrRecv[6], arrRecv[7], ref strOutData);
        //                        //strRet = "";
        //                        //strOutData = "850300500602002B0020230200002021020000000002000000100200000011020000001202000000130200000020020000002102000000220200000023020000003002000000400200000050020000005102000000520200000053020000006002000000610200000062020000006302000000700200000071020000007202000000730200000080020000008102000000820200000083020000009002000000910200000092020000009302000000A002000000A102000000A202000000A30200002031020000101002000010200200002131020200213202020021330202010106000000011C07E108010000000105050000152D050000041805000009C2050000025905000004F90105060000152D060000041806000009C2060000025906000004F9010106000006F40101060000071C0101060000071C010506000000000600000000060000000006000000000600000000010106000000000101060000000001010600000000010505000001AF050000005005000000CB05000000330500000061010505000000000500000000050000000005000000000500000000010506000001AF060000005006000000CB060000003306000000610101060000008E010106000000900101060000008F0105060000000006000000000600000000060000000006000000000101060000000001010600000000010106000000000105060000000006000000000600000000060000000006000000000101060000000001010600000000010106000000000105060000000006000000000600000000060000000006000000000101060000000001010600000000010106000000000105060000153F060000041B06000009CB060000025B06000004FC010106000006FA010106000007210101060000072101050600000000060000000006000000000600000000060000000001010600000000010106000000000101060000000006000000000105020206000096991C07E10803123200020206000091781C07E108060E3B2E0202060000943C1C07E10803113B00020206000096991C07E1080312320002020600008C301C07E108080A35090105020206000000001C00000000000000020206000000001C00000000000000020206000000001C00000000000000020206000000001C00000000000000020206000000001C00000000000000020506000002EC12000012271006000002EC0600000000020506000002FF12000012271006000002FF0600000000020506000002FF12000012271006000002FF06000000000000";
        //                    }
        //                    catch
        //                    {
        //                        strRet = "F7";
        //                    }
        //                    #endregion
        //                    break;
        //                case "37":
        //                    #region 广播数据加密
        //                    try
        //                    {
        //                        string sOutSID = "";
        //                        string sOutAttachData ="";
        //                        string sOutData = "";
        //                        string sOutMac = "";
        //                        strRet = Lib_GuoWang698_2016.fnObj_Meter_Formal_GetGrpBrdCstData(iFlag, int.Parse(arrRecv[3]), arrRecv[4], arrRecv[5], arrRecv[6], arrRecv[7], ref sOutSID, ref sOutAttachData, ref sOutData, ref sOutMac);
        //                        strOutData = sOutSID + "," + sOutAttachData + "," + sOutData+ "," + sOutMac;
        //                    }
        //                    catch
        //                    {
        //                        strRet = "F8";
        //                    }
        //                    #endregion
        //                    break;
        //                case "38":
        //                    #region 设置ESAM参数
        //                    try
        //                    {
        //                        string sOutSID = "";
        //                        string sOutAttachData = "";
        //                        string sOutData = "";
        //                        string sOutMac ="";
        //                        strRet = Lib_GuoWang698_2016.fnObj_Meter_Formal_SetESAMData(iFlag, int.Parse(arrRecv[3]), arrRecv[4], arrRecv[5], arrRecv[6], arrRecv[7], arrRecv[8], ref sOutSID, ref sOutAttachData, ref sOutData, ref sOutMac);
        //                        strOutData = sOutSID + "," + sOutAttachData+ "," + sOutData + "," + sOutMac;
        //                    }
        //                    catch
        //                    {
        //                        strRet = "F9";
        //                    }
        //                    #endregion
        //                    break;
        //                case "39":
        //                    #region 钱包操作
        //                    try
        //                    {
        //                        string sOutSID = "";
        //                        string sOutAttachData = "";
        //                        string sOutData = "";
        //                        string sOutMac = "";
        //                        strRet = Lib_GuoWang698_2016.fnObj_Meter_Formal_GetPurseData(iFlag, arrRecv[3], arrRecv[4], int.Parse(arrRecv[5]), arrRecv[6], ref sOutSID, ref sOutAttachData, ref sOutData, ref sOutMac);
        //                        strOutData = sOutSID + "," + sOutAttachData + "," +sOutData + "," + sOutMac;
        //                    }
        //                    catch
        //                    {
        //                        strRet = "FA";
        //                    }
        //                    #endregion
        //                    break;
        //                case "40":
        //                    #region 电能表对称密钥更新 正式密钥（厂家机密机不支持）
        //                    try
        //                    {
        //                        string sOutSID ="";
        //                        string sOutAttachData = "";
        //                        string sOutTrmKeyData = "";
        //                        string sOutMac ="";
        //                        strRet = Lib_GuoWang698_2016.fnObj_Meter_Formal_GetTrmKeyData(iFlag,arrRecv[3], arrRecv[4], arrRecv[5], arrRecv[6],ref sOutSID,ref sOutAttachData,ref sOutTrmKeyData,ref sOutMac);
        //                        strOutData = sOutSID + "," + sOutAttachData + "," + sOutTrmKeyData + "," + sOutMac;
        //                    }
        //                    catch
        //                    {
        //                        strRet = "FB";
        //                    }
        //                    #endregion
        //                    break;
        //                case "41":
        //                    #region 电能表对称初始化
        //                    try
        //                    {
        //                        string sOutSID = "";
        //                        string sOutAttachData = "";
        //                        string sOutTrmKeyData = "";
        //                        string sOutMac = "";
        //                        strRet = Lib_GuoWang698_2016.fnObj_Meter_Formal_InitTrmKeyData(iFlag, arrRecv[3], arrRecv[4], arrRecv[5], arrRecv[6], ref sOutSID, ref sOutAttachData, ref sOutTrmKeyData, ref sOutMac);
        //                        strOutData = sOutSID + "," + sOutAttachData+ "," + sOutTrmKeyData + "," + sOutMac;
        //                    }
        //                    catch
        //                    {
        //                        strRet = "FC";
        //                    }
        //                    #endregion
        //                    break;
        //                case "42":
        //                    #region 创建随机数
        //                    try
        //                    {
        //                        string sOutRand1="";
        //                        strRet = Lib_GuoWang698_2016.fnCreateRand(iFlag,ref sOutRand1);
        //                        strOutData = sOutRand1;
        //                    }
        //                    catch
        //                    {
        //                        strRet = "FD";
        //                    }
        //                    #endregion
        //                    break;
        //                case "43":
        //                    #region 红外认证函数
        //                    try
        //                    {
        //                        string sOutRand2Endata = "";
        //                        strRet = Lib_GuoWang698_2016.fnMeter_Formal_InfraredAuth(iFlag, arrRecv[3], arrRecv[4], arrRecv[5], arrRecv[6], arrRecv[7], ref sOutRand2Endata);
        //                        strOutData = sOutRand2Endata;
        //                    }
        //                    catch
        //                    {
        //                        strRet = "FE";
        //                    }
        //                    #endregion
        //                    break;
        //                case "44":
        //                    #region 抄读ESAM参数
        //                    try
        //                    {
        //                        string sOutRandHost = "";
        //                        string sOutSID = "";
        //                        string sOutAttachData = "";
        //                        strRet = Lib_GuoWang698_2016.fnObj_Meter_Formal_GetESAMData(iFlag,int.Parse(arrRecv[3]),arrRecv[4],arrRecv[5],ref sOutRandHost,ref sOutSID,ref sOutAttachData);
        //                        strOutData = sOutRandHost + "," + sOutSID + "," + sOutAttachData;
        //                    }
        //                    catch
        //                    {
        //                        strRet = "E0";
        //                    }
        //                    #endregion
        //                    break;
        //                case "45":
        //                    #region 抄读ESAM参数验证
        //                    try
        //                    {
        //                        string sOutData = "";
        //                        strRet = Lib_GuoWang698_2016.fnObj_Meter_Formal_VerifyESAMData(iFlag, int.Parse(arrRecv[3]), arrRecv[4], arrRecv[5], arrRecv[6], arrRecv[7], arrRecv[8],arrRecv[9], ref sOutData);
        //                        strOutData = sOutData;
        //                    }
        //                    catch
        //                    {
        //                        strRet = "E1";
        //                    }
        //                    #endregion
        //                    break;
        //                case "46":
        //                    #region 电能表对称密钥更新 测试密钥
        //                    try
        //                    {
        //                        string sOutSID = "";
        //                        string sOutAttachData = "";
        //                        string sOutTrmKeyData = "";
        //                        string sOutMac = "";
        //                        strRet = Lib_GuoWang698_2016.fnObj_Meter_Test_GetTrmKeyData(iFlag, arrRecv[3], arrRecv[4], arrRecv[5], arrRecv[6], ref sOutSID, ref sOutAttachData, ref sOutTrmKeyData, ref sOutMac);
        //                        strOutData = sOutSID + "," + sOutAttachData + "," + sOutTrmKeyData + "," + sOutMac;
        //                    }
        //                    catch
        //                    {
        //                        strRet = "E2";
        //                    }
        //                    #endregion
        //                    break;
        //                case "47":
        //                #region 抄读ESAM中文件的数据
        //                    try
        //                    {
        //                        //string cOutRandHost, ref string cOutSID, ref string cOutAttachData
        //                        string cOutRandHost = "";
        //                        string cOutSID = "";
        //                        string cOutAttachData = "";
        //                        strRet = Lib_GuoWang698_2016.fnObj_Meter_Formal_GetESAMFileData(iFlag, int.Parse(arrRecv[3]), arrRecv[4], arrRecv[5], arrRecv[6], ref cOutRandHost, ref cOutSID, ref cOutAttachData);
        //                        strOutData = cOutRandHost + "," + cOutSID + "," + cOutAttachData ;
        //                    }
        //                    catch
        //                    {
        //                        strRet = "E2";
        //                    }
        //                #endregion
        //                    break;
        //                case "48":
        //                    #region 抄读ESAM参数验证-测试密钥
        //                    try
        //                    {
        //                        string sOutData = "";
        //                        strRet = Lib_GuoWang698_2016.fnObj_Meter_Test_VerifyESAMData(iFlag, int.Parse(arrRecv[3]), arrRecv[4], arrRecv[5], arrRecv[6], arrRecv[7], arrRecv[8], arrRecv[9], ref sOutData);
        //                        strOutData = sOutData;
        //                    }
        //                    catch
        //                    {
        //                        strRet = "E1";
        //                    }
        //                    #endregion
        //                    break;
        //            }
        //            #endregion
        //        }
        //    }
        //    else
        //        strRet="ss";


        //    return strRet;
        //}
      
    }
}
