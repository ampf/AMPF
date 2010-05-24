<#setting number_format=",###">
<#escape x as x?html>

<h1>Valgrind Html Report for PluginSuite ${date?datetime}</h1>

<h2>Summary Statistics</h2>
<table border=1 cellspacing="0" cellpadding="0">
<tr>
  <th>#tests</th>
  <th>Definitely Lost Bytes</th>
  <th>Possibly Lost Bytes</th>
  <th>#errors</th>
  <th>#warnings</th>
</tr>
<tr>
  <td>${stats.testCount?number}</td>
  <td>${stats.lostBytes?number}</td>
  <td>${stats.possiblyLostBytes?number}</td>
  <td>${stats.errors?number}</td>
  <td>${stats.warnings?number}</td>
</tr>
</table>

<h2>Test Details</h2>

<table border=1 cellspacing="0" cellpadding="2">
<tr>
  <th>test</th>
  <#list details[0]?keys?sort as k>
  <#if k!="test">
  <th>${k}</th>
  </#if>
  </#list>
</tr>
<#list details?sort_by("lostBytes")?reverse as d>
<tr>
  <td><a href="${d.test?replace("junit_results","..")}.out">${d.test}</a></td>
  <#list d?keys?sort as k>
  <#if k!="test">
  <td>
  <a href="${d.test?replace("junit_results","..")}.err">
  ${d[k]}
  </td>
  </#if>
  </#list>
</tr>
</#list>
</table>

<h2> Binary Data in Output</h2>
<ul>
<#list files as f>
  <#if f?contains(": data")>
  <li>${f}</li>
  </#if>
</#list>
</ul>

<h2>Result Details</h2>

<table border=1 cellspacing="0" cellpadding"0">
<tr>
  <th>test</td>
  <th>lost</td>
  <th>errors</td>
  <th>warnings</td>
</tr>
<tr>
<#list tests as test>
<#assign data=model[test]>
<tr>
  <td>${test}&nbsp;
    <br/><a href="${data.stdout?replace("junit_results","..")}">stdout</a> <a href="${data.stderr?replace("junit_results","..")}">stderr</a>
  </td>
  <td>${data.lost}&nbsp;</td>
  <td>${data.errors}&nbsp;</td>
  <td>${data.warnings}&nbsp;</td>
</tr>
</#list>
</table>
</#escape>

