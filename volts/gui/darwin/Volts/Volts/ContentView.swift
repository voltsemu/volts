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
    @State var games: [Game] = [
        Game(name: "aaa", image: "aaa"),
        Game(name: "bbb", image: "bbb"),
        Game(name: "ccc", image: "ccc"),
        Game(name: "ddd", image: "ddd")
    ]
    
    var body: some View {
        ScrollView {
            VStack {
                ForEach(0 ..< self.games.count) { idx in
                    self.games[idx].frame(width: 200, height: 100)
                }
            }
        }.frame(minWidth: 0, maxWidth: .infinity, minHeight: 0, maxHeight: .infinity, alignment: .topLeading)
    }
}

struct ManageView: View {
    var body: some View {
        Text("Manage")
    }
}

struct SettingsView: View {
    var body: some View {
        HStack {
            VStack {
                Text("GPU")
            }
            Divider()
            VStack {
                Text("CPU")
            }
        }
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
