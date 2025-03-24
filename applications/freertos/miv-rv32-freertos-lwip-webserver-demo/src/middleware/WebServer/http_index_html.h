
/*******************************************************************************
 * Copyright 2025 Microchip FPGA Embedded Systems Solutions.
 *
 * SPDX-License-Identifier: MIT
 *
 * @file http_index_html.h
 * @author Microchip FPGA Embedded Systems Solutions
 * @brief HTML setting up the webserver.
 *
 */

#ifndef HTTP_INDEX_HTML_H
#define HTTP_INDEX_HTML_H

#include <stdint.h>

const static uint8_t http_index_html[] =
    "<!DOCTYPE html PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\" \"http://www.w3.org/TR/html4/loose.dtd\">\
<html dir=\"ltr\"><head>\
    <meta content=\"text/html; charset=UTF-8\" http-equiv=\"content-type\">\
    <title>Microchip CoreTSE Demo</title>\
\
    <meta content=\"Microchip\" name=\"author\"><meta content=\"BlueGriffon\
      wysiwyg editor\" name=\"generator\">\
    <style type=\"text/css\">\
        .bodyText {\
            font-family: Arial, Helvetica, sans-serif;\
            font-size: 12px;\
            color: #333333;\
        }\
        .headline2 {\
            font-family: Arial, Helvetica, sans-serif;\
            font-size: 12px;\
            font-weight: bold;\
            color: #333333;\
        }\
        .headline1 {\
            font-family: Arial, Helvetica, sans-serif;\
            font-size: 18px;\
            font-weight: bold;\
            color: #666666;\
        }\
        .smallText {\
            font-family: Arial, Helvetica, sans-serif;\
            font-size: 9px;\
            color: #666666;\
        }\
        .top_headline {\
            font-family: Arial, Helvetica, sans-serif;\
            font-size: 24px;\
            color: #666666;\
            padding-top: 5px;\
        }\
        </style>\
  </head>\
<body>\
<table border=\"0\" width=\"900\" align=\"center\">\r\
  <tbody>\r\
    <tr>\r\
        <td width=\"100%\"><p class=\"top_headline\" style=\"text-align: center;\">  Mi-V Soft processor FreeRTOS lwIP Webserver Example</p></td>\r\
    </tr>\r\
  </tbody>\r\
</table>\r\
<hr width=\"900\" />\r\
<table border=\"0\" width=\"900\" align=\"center\">\r\
      <tbody>\r\
        <tr>\r\
          <td width=\"10%\"  valign=\"top\" colspan=\"1\" rowspan=\"2\"><div class=\"headline1\" style=\"text-align: center;\"><span style=\"font-weight: bold;\">Ethernet Interface</span><br></div>\r\
            <table border=\"1\" align=\"center\" cellspacing=\"10\" style=\"border-collapse:collapse;\">\r\
              <caption>Port Parameters</caption>\r\
                <tbody class=\"bodyText\">\r\
                <tr>\r\
                  <td class=\"headline2\" style=\"text-align: right;\">MAC Address:<br></td>\r\
                  <td id=\"M_A\"  style=\"text-align: center;\">-<br></td>\r\
                </tr>\r\
                <tr>\r\
                  <td class=\"headline2\" style=\"text-align: right;\">TCP/IP Address:<br></td>\r\
                  <td id=\"TCPIP_Addr\" style=\"text-align: center;\">-<br></td>\r\
                </tr>\r\
                <tr>\r\
                  <td class=\"headline2\" style=\"text-align: right;\">Speed:<br></td>\r\
                  <td id=\"LinkSpeed\" style=\"text-align: center;\">-<br></td>\r\
                </tr>\r\
                <tr>\r\
                  <td class=\"headline2\" style=\"text-align: right;\">Duplex Mode:<br></td>\r\
                  <td id=\"DPM\" style=\"text-align: center;\">-<br></td>\r\
                </tr>\r\
              </tbody>\r\
            </table>\r\
          </td>\r\
          <td width=\"90%\">\r\
            <div class=\"headline1\" style=\"text-align: center;\"><span style=\"font-weight: bold;\">CoreTSE Statistics</span><br>\r\
            </div>\r\
            <table>\r\
            <tr>\r\
              <td width=\"30%\"  valign=\"top\" >\r\
                <table border=\"0\" align=\"center\" cellspacing=\"10\">\r\
                  <caption>Receive statistics</caption>\r\
                  <tbody class=\"bodyText\">\r\
                    <tr>\r\
                      <td><div class=\"headline2\" style=\"text-align: left;\">RX_BYTE_CNT:<br></div></td>\r\
                      <td><div id=\"S0\" style=\"text-align: center;\">-<br></div></td>\r\
                    </tr>\r\
                    <tr>\r\
                      <td><div class=\"headline2\" style=\"text-align: left;\">RX_PKT_CNT:<br></div></td>\r\
                      <td><div id=\"S1\" style=\"text-align: center;\">-<br></div></td>\r\
                    </tr>\r\
                    <tr>\r\
                      <td><div class=\"headline2\" style=\"text-align: left;\">RX_FCS_ERR_CNT:<br></div></td>\r\
                      <td><div id=\"S2\" style=\"text-align: center;\">-<br></div></td>\r\
                    </tr>\r\
                    <tr>\r\
                      <td><div class=\"headline2\" style=\"text-align: left;\">RX_MULTICAST_PKT_CNT:<br></div></td>\r\
                      <td><div id=\"S3\" style=\"text-align: center;\">-<br></div></td>\r\
                    </tr>\r\
                    <tr>\r\
                      <td><div class=\"headline2\" style=\"text-align: left;\">RX_BROADCAST_PKT_CNT:<br></div></td>\r\
                      <td><div id=\"S4\" style=\"text-align: center;\">-<br></div></td>\r\
                    </tr>\r\
                    <tr>\r\
                      <td><div class=\"headline2\" style=\"text-align: left;\">RX_CTRL_PKT_CNT:<br></div></td>\r\
                      <td><div id=\"S5\" style=\"text-align: center;\">-<br></div></td>\r\
                    </tr>\r\
                    <tr>\r\
                      <td><div class=\"headline2\" style=\"text-align: left;\">RX_PAUSE_PKT_CNT:<br></div></td>\r\
                      <td><div id=\"S6\" style=\"text-align: center;\">-<br></div></td>\r\
                    </tr>\r\
                    <tr>\r\
                      <td><div class=\"headline2\" style=\"text-align: left;\">RX_UNKNOWN_OPCODE_CNT:<br></div></td>\r\
                      <td><div id=\"S7\" style=\"text-align: center;\">-<br></div></td>\r\
                    </tr>\r\
                    <tr>\r\
                      <td><div class=\"headline2\" style=\"text-align: left;\">RX_ALIGN_ERR_CNT:<br></div></td>\r\
                      <td><div id=\"S8\" style=\"text-align: center;\">-<br></div></td>\r\
                    </tr>\r\
                    <tr>\r\
                      <td><div class=\"headline2\" style=\"text-align: left;\">RX_FRAMELENGTH_ERR_CNT:<br></div></td>\r\
                      <td><div id=\"S9\" style=\"text-align: center;\">-<br></div></td>\r\
                    </tr>\r\
                    <tr>\r\
                      <td><div class=\"headline2\" style=\"text-align: left;\">RX_CODE_ERR_CNT:<br></div></td>\r\
                      <td><div id=\"S10\" style=\"text-align: center;\">-<br></div></td>\r\
                    </tr>\r\
                    <tr>\r\
                      <td><div class=\"headline2\" style=\"text-align: left;\">RX_CS_ERR_CNT:<br></div></td>\r\
                      <td><div id=\"S11\" style=\"text-align: center;\">-<br></div></td>\r\
                    </tr>\r\
                    <tr>\r\
                      <td><div class=\"headline2\" style=\"text-align: left;\">RX_UNDERSIZE_PKT_CNT:<br></div></td>\r\
                      <td><div id=\"S12\" style=\"text-align: center;\">-<br></div></td>\r\
                    </tr>\r\
                    <tr>\r\
                      <td><div class=\"headline2\" style=\"text-align: left;\">RX_OVERSIZE_PKT_CNT:<br></div></td>\r\
                      <td><div id=\"S13\" style=\"text-align: center;\">-<br></div></td>\r\
                    </tr>\r\
                    <tr>\r\
                      <td><div class=\"headline2\" style=\"text-align: left;\">RX_FRAGMENT_CNT:<br></div></td>\r\
                      <td><div id=\"S14\" style=\"text-align: center;\">-<br></div></td>\r\
                    </tr>\r\
                    <tr>\r\
                      <td><div class=\"headline2\" style=\"text-align: left;\">RX_JABBER_CNT:<br></div></td>\r\
                      <td><div id=\"S15\" style=\"text-align: center;\">-<br></div></td>\r\
                    </tr>\r\
                    <tr>\r\
                      <td><div class=\"headline2\" style=\"text-align: left;\">RX_DROP_CNT:<br></div></td>\r\
                      <td><div id=\"S16\" style=\"text-align: center;\">-<br></div></td>\r\
                    </tr>\r\
                  </tbody>\r\
                </table>\r\
              </td>\r\
            <td width=\"30%\" valign=\"top\" >\r\
                <table border=\"0\" width=\"33%\" align=\"center\" cellspacing=\"10\">\r\
                  <caption>TX-RX statistics</caption>\r\
                  <tbody class=\"bodyText\">\r\
                    <tr>\r\
                      <td width=\"28%\"><div class=\"headline2\" style=\"text-align: left;\">FRAME_CNT_64:<br></div></td>\r\
                      <td width=\"5%\"><div id=\"S17\" style=\"text-align: center;\">-<br></div></td>\r\
                    </tr>\r\
                    <tr>\r\
                      <td><div class=\"headline2\" style=\"text-align: left;\">FRAME_CNT_127:<br></div></td>\r\
                      <td><div id=\"S18\" style=\"text-align: center;\">-<br></div></td>\r\
                    </tr>\r\
                    <tr>\r\
                      <td><div class=\"headline2\" style=\"text-align: left;\">FRAME_CNT_255:<br></div></td>\r\
                      <td><div id=\"S19\" style=\"text-align: center;\">-<br></div></td>\r\
                    </tr>\r\
                    <tr>\r\
                      <td><div class=\"headline2\" style=\"text-align: left;\">FRAME_CNT_511:<br></div></td>\r\
                      <td><div id=\"S20\" style=\"text-align: center;\">-<br></div></td>\r\
                    </tr>\r\
                    <tr>\r\
                      <td><div class=\"headline2\" style=\"text-align: left;\">FRAME_CNT_1K:<br></div></td>\r\
                      <td><div id=\"S21\" style=\"text-align: center;\">-<br></div></td>\r\
                    </tr>\r\
                    <tr>\r\
                      <td><div class=\"headline2\" style=\"text-align: left;\">FRAME_CNT_MAX:<br></div></td>\r\
                      <td><div id=\"S22\" style=\"text-align: center;\">-<br></div></td>\r\
                    </tr>\r\
                    <tr>\r\
                      <td><div class=\"headline2\" style=\"text-align: left;\">FRAME_CNT_VLAN:<br></div></td>\r\
                      <td><div id=\"S23\" style=\"text-align: center;\">-<br></div></td>\r\
                    </tr>\r\
                 </tbody>\r\
                </table>\r\
              </td>\r\
            <td width=\"30%\"  valign=\"top\" >\r\
                <table border=\"0\" width=\"33%\" align=\"center\" cellspacing=\"10\">\r\
                 <caption>Transmit statistics</caption>\r\
                  <tbody class=\"bodyText\">\r\
                    <tr>\r\
                      <td><div class=\"headline2\" style=\"text-align: left;\">TX_BYTE_CNT:<br></div></td>\r\
                      <td><div id=\"S24\" style=\"text-align: center;\">-<br></div></td>\r\
                    </tr>\r\
                    <tr>\r\
                      <td><div class=\"headline2\" style=\"text-align: left;\">TX_PKT_CNT:<br></div></td>\r\
                      <td><div id=\"S25\" style=\"text-align: center;\">-<br></div></td>\r\
                    </tr>\r\
                    <tr>\r\
                      <td><div class=\"headline2\" style=\"text-align: left;\">TX_MULTICAST_PKT_CNT:<br></div></td>\r\
                      <td><div id=\"S26\" style=\"text-align: center;\">-<br></div></td>\r\
                    </tr>\r\
                    <tr>\r\
                      <td><div class=\"headline2\" style=\"text-align: left;\">TX_BROADCAST_PKT_CNT:<br></div></td>\r\
                      <td><div id=\"S27\" style=\"text-align: center;\">-<br></div></td>\r\
                    </tr>\r\
                    <tr>\r\
                      <td><div class=\"headline2\" style=\"text-align: left;\">TX_PAUSE_PKT_CNT:<br></div></td>\r\
                      <td><div id=\"S28\" style=\"text-align: center;\">-<br></div></td>\r\
                    </tr>\r\
                    <tr>\r\
                      <td><div class=\"headline2\" style=\"text-align: left;\">TX_DEFFERAL_PKT_CNT:<br></div></td>\r\
                      <td><div id=\"S29\" style=\"text-align: center;\">-<br></div></td>\r\
                    </tr>\r\
                    <tr>\r\
                      <td><div class=\"headline2\" style=\"text-align: left;\">TX_EXCS_DEFFERAL_PKT_CNT:<br></div></td>\r\
                      <td><div id=\"S30\" style=\"text-align: center;\">-<br></div></td>\r\
                    </tr>\r\
                    <tr>\r\
                      <td><div class=\"headline2\" style=\"text-align: left;\">TX_SINGLE_COLL_PKT_CNT:<br></div></td>\r\
                      <td><div id=\"S31\" style=\"text-align: center;\">-<br></div></td>\r\
                    </tr>\r\
                    <tr>\r\
                      <td><div class=\"headline2\" style=\"text-align: left;\">TX_MULTI_COLL_PKT_CNT:<br></div></td>\r\
                      <td><div id=\"S32\" style=\"text-align: center;\">-<br></div></td>\r\
                    </tr>\r\
                    <tr>\r\
                      <td><div class=\"headline2\" style=\"text-align: left;\">TX_LATE_COLL_PKT_CNT:<br></div></td>\r\
                      <td><div id=\"S33\" style=\"text-align: center;\">-<br></div></td>\r\
                    </tr>\r\
                    <tr>\r\
                      <td><div class=\"headline2\" style=\"text-align: left;\">TX_EXCSS_COLL_PKT_CNT:<br></div></td>\r\
                      <td><div id=\"S34\" style=\"text-align: center;\">-<br></div></td>\r\
                    </tr>\r\
                    <tr>\r\
                      <td><div class=\"headline2\" style=\"text-align: left;\">TX_TOTAL_COLL_PKT_CNT:<br></div></td>\r\
                      <td><div id=\"S35\" style=\"text-align: center;\">-<br></div></td>\r\
                    </tr>\r\
                    <tr>\r\
                      <td><div class=\"headline2\" style=\"text-align: left;\">TX_PAUSE_HONORED_CNT:<br></div></td>\r\
                      <td><div id=\"S36\" style=\"text-align: center;\">-<br></div></td>\r\
                    </tr>\r\
                    <tr>\r\
                      <td><div class=\"headline2\" style=\"text-align: left;\">TX_DROP_CNT:<br></div></td>\r\
                      <td><div id=\"S37\" style=\"text-align: center;\">-<br></div></td>\r\
                    </tr>\r\
                    <tr>\r\
                      <td><div class=\"headline2\" style=\"text-align: left;\">TX_JABBER_CNT:<br></div></td>\r\
                      <td><div id=\"S38\" style=\"text-align: center;\">-<br></div></td>\r\
                    </tr>\r\
                    <tr>\r\
                      <td><div class=\"headline2\" style=\"text-align: left;\">TX_FCS_ERR_CNT:<br></div></td>\r\
                      <td><div id=\"S39\" style=\"text-align: center;\">-<br></div></td>\r\
                    </tr>\r\
                    <tr>\r\
                      <td><div class=\"headline2\" style=\"text-align: left;\">TX_CNTRL_PKT_CNT:<br></div></td>\r\
                      <td><div id=\"S40\" style=\"text-align: center;\">-<br></div></td>\r\
                    </tr>\r\
                    <tr>\r\
                      <td><div class=\"headline2\" style=\"text-align: left;\">TX_OVERSIZE_PKT_CNT:<br></div></td>\r\
                      <td><div id=\"S41\" style=\"text-align: center;\">-<br></div></td>\r\
                    </tr>\r\
                    <tr>\r\
                      <td><div class=\"headline2\" style=\"text-align: left;\">TX_UNDERSIZE_PKT_CNT:<br></div></td>\r\
                      <td><div id=\"S42\" style=\"text-align: center;\">-<br></div></td>\r\
                    </tr>\r\
                    <tr>\r\
                      <td><div class=\"headline2\" style=\"text-align: left;\">TX_FRAGMENT_CNT:<br></div></td>\r\
                      <td><div id=\"S43\" style=\"text-align: center;\">-<br></div></td>\r\
                    </tr>\r\
                  </tbody>\r\
                </table>\r\
              </td>\r\
              </tr>\r\
            </table>\r\
          </td>\r\
        </tr>\r\
        <tr>\r\
          <td>\r\
            <hr />\r\
            <table border=\"0\" width=\"65%\" align=\"center\" cellspacing=\"10\">\r\
              <tbody class=\"bodyText\">\r\
                <tr>\r\
                  <td>\r\
                    <table border=\"0\" width=\"100%\" align=\"center\">\r\
                      <tbody class=\"bodyText\">\r\
                        <tr>\r\
                            <td align=\"center\"><button onclick=\"post_clear_stats_request();\">Clear Statistics</button></td>\r\
                        </tr>\r\
                      </tbody>\r\
                    </table>\r\
                    </td>\r\
                </tr>\r\
                <tr>\r\
                  <td>\r\
                    </td>\r\
                </tr>\r\
              </tbody>\r\
            </table>\r\
          </td>\r\
        </tr>\r\
      </tbody>\r\
    \r\
</table>\r\
<hr width=\"900\" />\r\
<div class=\"smallText\" width=\"50%\" style=\"text-align: center; font-weight: bold;\">Copyright Microchip FPGA Embedded Systems Solutions - v1.0<br></div>\r\
<p><br>\
    </p>\r\
<script type=\"text/javascript\">\r\
function update_page() {\r\
    var request = new XMLHttpRequest();\r\
    request.open(\"GET\",\"status\");\r\
    request.onreadystatechange = function() {\r\
        if(request.readyState === 4 && request.status === 200) {\r\
            var parsed_status = JSON.parse(request.responseText);\r\
            var mac_addr = document.getElementById(\"M_A\");\r\
            mac_addr.innerHTML = parsed_status.M_A;\r\
            var tcpip_addr = document.getElementById(\"TCPIP_Addr\");\r\
            tcpip_addr.innerHTML = parsed_status.TCPIP_Addr;\r\
            var link_speed = document.getElementById(\"LinkSpeed\");\r\
            link_speed.innerHTML = parsed_status.LinkSpeed;\r\
            var duplex_mode = document.getElementById(\"DPM\");\r\
            duplex_mode.innerHTML = parsed_status.DPM;\r\
            var mac_stat_7 = document.getElementById(\"S0\");\r\
            mac_stat_7.innerHTML = parsed_status.S0;\r\
            var mac_stat_8 = document.getElementById(\"S1\");\r\
            mac_stat_8.innerHTML = parsed_status.S1;\r\
            var mac_stat_9 = document.getElementById(\"S2\");\r\
            mac_stat_9.innerHTML = parsed_status.S2;\r\
            var mac_stat_10 = document.getElementById(\"S3\");\r\
            mac_stat_10.innerHTML = parsed_status.S3;\r\
            var mac_stat_11 = document.getElementById(\"S4\");\r\
            mac_stat_11.innerHTML = parsed_status.S4;\r\
            var mac_stat_12 = document.getElementById(\"S5\");\r\
            mac_stat_12.innerHTML = parsed_status.S5;\r\
            var mac_stat_13 = document.getElementById(\"S6\");\r\
            mac_stat_13.innerHTML = parsed_status.S6;\r\
            var mac_stat_14 = document.getElementById(\"S7\");\r\
            mac_stat_14.innerHTML = parsed_status.S7;\r\
            var mac_stat_15 = document.getElementById(\"S8\");\r\
            mac_stat_15.innerHTML = parsed_status.S8;\r\
            var mac_stat_16 = document.getElementById(\"S9\");\r\
            mac_stat_16.innerHTML = parsed_status.S9;\r\
            var mac_stat_17 = document.getElementById(\"S10\");\r\
            mac_stat_17.innerHTML = parsed_status.S10;\r\
            var mac_stat_18 = document.getElementById(\"S11\");\r\
            mac_stat_18.innerHTML = parsed_status.S11;\r\
            var mac_stat_19 = document.getElementById(\"S12\");\r\
            mac_stat_19.innerHTML = parsed_status.S12;\r\
            var mac_stat_20 = document.getElementById(\"S13\");\r\
            mac_stat_20.innerHTML = parsed_status.S13;\r\
            var mac_stat_21 = document.getElementById(\"S14\");\r\
            mac_stat_21.innerHTML = parsed_status.S14;\r\
            var mac_stat_22 = document.getElementById(\"S15\");\r\
            mac_stat_22.innerHTML = parsed_status.S15;\r\
            var mac_stat_23 = document.getElementById(\"S16\");\r\
            mac_stat_23.innerHTML = parsed_status.S16;\r\
            var mac_stat_0 = document.getElementById(\"S17\");\r\
            mac_stat_0.innerHTML = parsed_status.S17;\r\
            var mac_stat_1 = document.getElementById(\"S18\");\r\
            mac_stat_1.innerHTML = parsed_status.S18;\r\
            var mac_stat_2 = document.getElementById(\"S19\");\r\
            mac_stat_2.innerHTML = parsed_status.S19;\r\
            var mac_stat_3 = document.getElementById(\"S20\");\r\
            mac_stat_3.innerHTML = parsed_status.S20;\r\
            var mac_stat_4 = document.getElementById(\"S21\");\r\
            mac_stat_4.innerHTML = parsed_status.S21;\r\
            var mac_stat_5 = document.getElementById(\"S22\");\r\
            mac_stat_5.innerHTML = parsed_status.S22;\r\
            var mac_stat_6 = document.getElementById(\"S23\");\r\
            mac_stat_6.innerHTML = parsed_status.S23;\r\
            var mac_stat_24 = document.getElementById(\"S24\");\r\
            mac_stat_24.innerHTML = parsed_status.S24;\r\
            var mac_stat_25 = document.getElementById(\"S25\");\r\
            mac_stat_25.innerHTML = parsed_status.S25;\r\
            var mac_stat_26 = document.getElementById(\"S26\");\r\
            mac_stat_26.innerHTML = parsed_status.S26;\r\
            var mac_stat_27 = document.getElementById(\"S27\");\r\
            mac_stat_27.innerHTML = parsed_status.S27;\r\
            var mac_stat_28 = document.getElementById(\"S28\");\r\
            mac_stat_28.innerHTML = parsed_status.S28;\r\
            var mac_stat_29 = document.getElementById(\"S29\");\r\
            mac_stat_29.innerHTML = parsed_status.S29;\r\
            var mac_stat_30 = document.getElementById(\"S30\");\r\
            mac_stat_30.innerHTML = parsed_status.S30;\r\
            var mac_stat_31 = document.getElementById(\"S31\");\r\
            mac_stat_31.innerHTML = parsed_status.S31;\r\
            var mac_stat_32 = document.getElementById(\"S32\");\r\
            mac_stat_32.innerHTML = parsed_status.S32;\r\
            var mac_stat_33 = document.getElementById(\"S33\");\r\
            mac_stat_33.innerHTML = parsed_status.S33;\r\
            var mac_stat_34 = document.getElementById(\"S34\");\r\
            mac_stat_34.innerHTML = parsed_status.S34;\r\
            var mac_stat_35 = document.getElementById(\"S35\");\r\
            mac_stat_35.innerHTML = parsed_status.S35;\r\
            var mac_stat_36 = document.getElementById(\"S36\");\r\
            mac_stat_36.innerHTML = parsed_status.S36;\r\
            var mac_stat_37 = document.getElementById(\"S37\");\r\
            mac_stat_37.innerHTML = parsed_status.S37;\r\
            var mac_stat_38 = document.getElementById(\"S38\");\r\
            mac_stat_38.innerHTML = parsed_status.S38;\r\
            var mac_stat_39 = document.getElementById(\"S39\");\r\
            mac_stat_39.innerHTML = parsed_status.S39;\r\
            var mac_stat_40 = document.getElementById(\"S40\");\r\
            mac_stat_40.innerHTML = parsed_status.S40;\r\
            var mac_stat_41 = document.getElementById(\"S41\");\r\
            mac_stat_41.innerHTML = parsed_status.S41;\r\
            var mac_stat_42 = document.getElementById(\"S42\");\r\
            mac_stat_42.innerHTML = parsed_status.S42;\r\
            var mac_stat_43 = document.getElementById(\"S43\");\r\
            mac_stat_43.innerHTML = parsed_status.S43;\r\
        };\r\
    };\r\
    request.send(null);\r\
};\r\
\r\
function encodeFormData(data) {\r\
    if (!data) return \"\";\r\
    var pairs = [];\r\
    for(var name in data) {\r\
        if (!data.hasOwnProperty(name)) continue;\r\
        if (typeof data[name] === \"function\") continue;\r\
        var value = data[name].toString();\r\
        name = encodeURIComponent(name.replace(\" \", \"+\"));\r\
        value = encodeURIComponent(value.replace(\" \", \"+\"));\r\
        pairs.push(name + \"=\" + value);\r\
    }\r\
    return pairs.join('&');\r\
}\r\
\r\
function post_clear_stats_request() {\r\
    var request = new XMLHttpRequest();\r\
    request.open(\"GET\",\"clear_stats?\");\r\
    request.send(null);\r\
};\r\
\r\
window.onload = function() {\r\
    document.getElementById(\"S17\").value = \"0\";\r\
    setInterval(update_page, 500);\r\
};\r\
</script>\r\
</body></html>";

#endif /* HTTP_INDEX_HTML_H */