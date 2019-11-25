//
//  ContentView.swift
//  Volts
//
//  Created by Student on 11/25/19.
//  Copyright © 2019 Apache. All rights reserved.
//

import SwiftUI

struct Game: View {
    let name: String
    let image: String
    
    var body: some View {
        VStack {
            Image(image).resizable().scaledToFit()
            Text(name).font(.headline)
        }
    }
}

struct GamesView: View {
    var body: some View {
        Text("Games")
    }
}

struct ManageView: View {
    var body: some View {
        Text("Manage")
    }
}

struct SettingsView: View {
    var body: some View {
        Text("Settings")
    }
}

struct ContentView: View {
    var body: some View {
        TabView {
            GamesView()
            .tabItem {
                Text("Games")
            }
            
            ManageView()
            .tabItem {
                Text("Manage")
            }
            
            SettingsView()
            .tabItem {
                Text("Settings")
            }
        }
    }
}


struct ContentView_Previews: PreviewProvider {
    static var previews: some View {
        ContentView()
    }
}
