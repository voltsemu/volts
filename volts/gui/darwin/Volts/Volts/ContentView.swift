//
//  ContentView.swift
//  Volts
//
//  Created by Student on 11/25/19.
//  Copyright © 2019 Apache. All rights reserved.
//

import SwiftUI

struct ContentView: View {
    var body: some View {
        VStack {
            HStack {
                Text("A")
                Text("B")
            }
            
            Text("C")
            
            HStack {
                Text("D")
                Text("E")
                Text("F")
            }
            
            Text("Hello World")
                .frame(maxWidth: .infinity, maxHeight: .infinity)
        }
    }
}


struct ContentView_Previews: PreviewProvider {
    static var previews: some View {
        ContentView()
    }
}
