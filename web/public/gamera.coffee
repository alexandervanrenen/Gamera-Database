server = "http://localhost:4567"

window.GameraModel = ->
   self = @
   @editor = ace.edit("editor")

   @g_result = ko.observable("gamera results")
   @s_result = ko.observable("sqlite results")
   @g_resultRows = ko.observableArray([])
   @s_resultRows = ko.observableArray([])

   @isLoading = ko.observable(false)
   @logState = ko.observable(false)
   @column = ko.observable(0)
   @line = ko.observable(1)

   @queryStr = ""
   @gameraStr = ""
   @sqliteStr = ""

   @sendQueryCompare = () ->
      if not self.isLoading() && not self.logState()
         self.queryStr = "" + self.editor.getSession()
         return alert "Use only single statements" if (self.queryStr.match(/;/g)||[]).length != 1
         self.isLoading(true)
         $.ajax({type: 'POST', url: server + "/query/compare", data: self.queryStr, dataType: "text", success: (response) -> self.receiveQueryLoggingResult(response) });

   @sendQuery = () ->
      if not self.isLoading()
         self.queryStr = "" + self.editor.getSession()
         self.isLoading(true)
         $.ajax({type: 'POST', url: server + "/query/execute", data: self.queryStr, dataType: "text", success: (response) -> self.receiveQueryResult(response) });

   @logGamera = () ->
      if not self.isLoading() && self.logState()
         self.isLoading(true)
         msg = {query: "" + self.queryStr, expected: self.gameraStr}
         $.ajax({type: 'POST', url: server + "/query/log", data: JSON.stringify(msg), dataType: "text", success: (response) -> window.setTimeout (() -> self.isLoading(false).logState(false)) , 1400 });

   @ignoreResult = () ->
      self.logState(false)

   @logSqlite = () ->
      if not self.isLoading() && self.logState()
         self.isLoading(true)
         msg = {query: "" + self.queryStr, expected: self.sqliteStr}
         $.ajax({type: 'POST', url: server + "/query/log", data: JSON.stringify(msg), dataType: "text", success: (response) -> window.setTimeout (() -> self.isLoading(false).logState(false)) , 1400 });

   @receiveQueryResult = (response) ->
      # Handle errors
      window.setTimeout (() -> self.isLoading(false).logState(true)) , 1400
      self.g_resultRows([[]]).s_resultRows([[]])
      return self.g_result("no response").s_result("no response") unless response? or response != ""
      data = JSON.parse response
      # --- Update GUI
      return self.g_result("error: " + data.error) if data.error? and data.error != ""
      return self.g_result("error: no statement was executed") if not data.results.length? or data.results.length == 0
      return self.g_result("Executed #{data.results.length} statements.") if data.results.length > 1
      lastResult = data.results[data.results.length-1]
      switch lastResult.type
         when "create" then return self.g_result("Created table '#{lastResult.table}' in #{lastResult.time}.")
         when "insert" then return self.g_result("Inserted one tuple into table '#{lastResult.table}' in #{lastResult.time}.")
         when "select"
            self.g_result("Fetched #{lastResult.content.length} tuple(s) in #{lastResult.time}")
            self.g_resultRows([lastResult.layout].concat(lastResult.content))
         else return self.result("Error unknown result type '#{lastResult.type}'")

   @receiveQueryLoggingResult = (response) ->
      # Handle errors
      window.setTimeout (() -> self.isLoading(false).logState(true)) , 1400
      self.g_resultRows([[]]).s_resultRows([[]])
      return self.g_result("no response").s_result("no response") unless response? or response != ""
      data = JSON.parse response
      # --- Do gamera
      doGamera = (data) ->
         self.gameraStr = JSON.stringify data
         return self.g_result("error: " + data.error) if data.error? and data.error != ""
         self.g_result("Success.")
         if data.content.length != 0
            header = []
            header = header.concat(["?"]) for d, i in data.content[0]
            self.g_resultRows([header].concat(data.content))
      doGamera data.gamera
      # --- Do sqlite
      doSqlite = (data) ->
         self.sqliteStr = JSON.stringify data
         return self.s_result("error: " + data.error) if data.error? and data.error != ""
         self.s_result("Success.")
         if data.content.length != 0
            header = []
            header = header.concat(["?"]) for d, i in data.content[0]
            self.s_resultRows([header].concat(data.content))
      doSqlite data.sqlite

   @initialize = ->
      # Set up editor
      self.editor.getSession().selection.on 'changeCursor', (ev) -> self.column(self.editor.selection.getCursor().column).line(self.editor.selection.getCursor().row+1)
      self.editor.setTheme("ace/theme/twilight");
      self.editor.getSession().setMode("ace/mode/mysql");
      self.editor.getSession().setTabSize(3);
      self.editor.getSession().setUseWrapMode(false);

   return
