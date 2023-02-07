idf_component_register(SRCS "app_main.c"
        "conn/intf_wifi.c"
        "conn/connectivity.c"
        "server/server.c"
        INCLUDE_DIRS "."
        EMBED_TXTFILES 
        "../webpages/index.html"
        "../webpages/app.js"
        "../webpages/config.json"
)

# target_add_binary_data(server "webpages/page1.html" TEXT)
